#!/usr/bin/env python3
import argparse
import os
import sys

from dataclasses import dataclass, field
import time
from typing import Callable, ClassVar, Dict, Optional


class TimerError(Exception):
    """A custom exception used to report errors in use of Timer class"""


@dataclass
class Timer:
    timers: ClassVar[Dict[str, float]] = {}
    name: Optional[str] = None
    msg: Optional[str] = None
    text: str = "Elapsed time: {:0.4f} seconds"
    logger: Optional[Callable[[str], None]] = print
    _start_time: Optional[float] = field(default=None, init=False, repr=False)

    def __post_init__(self) -> None:
        """Add timer to dict of timers after initialization"""
        if self.name is not None:
            self.timers.setdefault(self.name, 0)

    def start(self) -> None:
        """Start a new timer"""
        if self._start_time is not None:
            raise TimerError(f"Timer is running. Use .stop() to stop it")

        self._start_time = time.perf_counter()

    def stop(self) -> float:
        """Stop the timer, and report the elapsed time"""
        if self._start_time is None:
            raise TimerError(f"Timer is not running. Use .start() to start it")

        # Calculate elapsed time
        elapsed_time = time.perf_counter() - self._start_time
        self._start_time = None

        # Report elapsed time
        if self.logger:
            self.logger(self.msg if self.msg else "", self.text.format(elapsed_time))
        if self.name:
            self.timers[self.name] += elapsed_time

        return elapsed_time

    def __enter__(self):
        """Start a new timer as a context manager"""
        self.start()
        return self

    def __exit__(self, *exc_info):
        """Stop the context manager timer"""
        self.stop()


# only invoke earthaccess if the height tile file does not already exist
parser = argparse.ArgumentParser(description="A script that processes a file.")

# Add the positional argument
# Note the lack of a leading dash or double dash
parser.add_argument(
    "filename", type=str, help="The name of the file to process, ex. N00E109.SRTMGL3.hgt"
)

args = parser.parse_args()

print(f"Processing file: {args.filename}")

if os.path.exists(f"hgt_files/{args.filename}.zip"):
    print(f"File hgt_files/{args.filename}.zip exists, skipping download.")
    sys.exit(0)
else:
    print(f"File hgt_files/{args.filename}.zip does not exist. Proceeding to download.")
    with Timer(msg="import earthaccess:"):
        import earthaccess
        from earthaccess.results import DataGranule

    # check that Earthdata Login credentials are set as environment variables
    if not os.environ["EARTHDATA_USERNAME"] or not os.environ["EARTHDATA_PASSWORD"]:
        raise ValueError(
            "Please set the EARTHDATA_USERNAME and EARTHDATA_PASSWORD environment variables."
        )
    with Timer(msg="login to earthaccess:"):
        earthaccess.login()

    short_name = "SRTMGL3"
    version = "003"  # Example version
    granule_pattern = args.filename  # Example partial granule name search

    with Timer(msg="search earthaccess:"):
        results = earthaccess.search_data(
            short_name=short_name,
            version=version,
            granule_name=granule_pattern,
            cloud_hosted=True,  # Ensure data is cloud-hosted for direct S3 access
        )

    # The NASA SRTM data was not generated for ocean areas, so some tiles will not exist
    # when no results are found, exit with code 1
    print(f"Found {len(results)} granules matching the criteria.")
 
    if len(results) == 0:
        print(f"No results from search using '{args.filename}', skipping download.")
        print(f"File for '{args.filename}' may not exist/was not recorded, check NASA Earthdata site.")
        sys.exit(1)
    else:
        with Timer(msg="download from earthaccess:"):
            files = earthaccess.download(results, "./hgt_files")

        print("Downloaded files to ./hgt_files:")
        for f in files:
            print(f)
