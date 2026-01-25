#!/usr/bin/env python3
import argparse
import os
import pprint
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

    print(f"Found {len(results)} granules matching the criteria.")
    #for r in results:
        # print("result:", r)
        # print("raw:", repr(r))
        # print(f"Converting to standard dict: {dict(r)}")
        # print(f"Converting to pretty dict: {pprint.pprint(dict(r))}")

    sample_meta = {
        "collection-concept-id": "C2763266377-LPCLOUD",
        "concept-id": "G2822547092-LPCLOUD",
        "concept-type": "granule",
        "format": "application/echo10+xml",
        "native-id": "N00E109.SRTMGL3.hgt",
        "provider-id": "LPCLOUD",
        "revision-date": "2023-12-22T15:58:36.464Z",
        "revision-id": 1,
    }
    sample_size = (1.29836,)
    sample_umm = {
        "AdditionalAttributes": [
            {
                "Name": "identifier_product_doi_authority",
                "Values": ["http://dx.doi.org"],
            },
            {
                "Name": "identifier_product_doi",
                "Values": ["10.5067/MEaSUREs/SRTM/SRTMGL3.003"],
            },
        ],
        "CollectionReference": {"ShortName": "SRTMGL3", "Version": "003"},
        "DataGranule": {
            "ArchiveAndDistributionInformation": [
                {"Name": "Not provided", "Size": 1.29836, "SizeUnit": "MB"}
            ],
            "DayNightFlag": "Unspecified",
            "Identifiers": [
                {
                    "Identifier": "N00E109.SRTMGL3.hgt",
                    "IdentifierType": "ProducerGranuleId",
                }
            ],
            "ProductionDateTime": "2012-12-06T17:44:13.000Z",
        },
        "GranuleUR": "N00E109.SRTMGL3.hgt",
        "MetadataSpecification": {
            "Name": "UMM-G",
            "URL": "https://cdn.earthdata.nasa.gov/umm/granule/v1.6.6",
            "Version": "1.6.6",
        },
        "Projects": [{"Campaigns": ["SRTM"], "ShortName": "Not provided"}],
        "ProviderDates": [
            {"Date": "2013-10-28T13:21:57.660Z", "Type": "Insert"},
            {"Date": "2019-10-05T14:28:19.606Z", "Type": "Update"},
        ],
        "RelatedUrls": [
            {
                "Description": "Download N00E109.SRTMGL3.hgt.zip",
                "Type": "GET DATA",
                "URL": "https://data.lpdaac.earthdatacloud.nasa.gov/lp-prod-protected/SRTMGL3.003/N00E109.SRTMGL3.hgt/N00E109.SRTMGL3.hgt.zip",
            },
            {
                "Description": "This link provides direct download "
                "access via S3 to the granule",
                "Type": "GET DATA VIA DIRECT ACCESS",
                "URL": "s3://lp-prod-protected/SRTMGL3.003/N00E109.SRTMGL3.hgt/N00E109.SRTMGL3.hgt.zip",
            },
            {
                "Description": "Download N00E109.SRTMGL3.hgt.cmr.xml",
                "Type": "VIEW RELATED INFORMATION",
                "URL": "https://data.lpdaac.earthdatacloud.nasa.gov/lp-prod-protected/SRTMGL3.003/N00E109.SRTMGL3.hgt/N00E109.SRTMGL3.hgt.cmr.xml",
            },
            {
                "Description": "This link provides direct download "
                "access via S3 to the granule",
                "Type": "VIEW RELATED INFORMATION",
                "URL": "s3://lp-prod-protected/SRTMGL3.003/N00E109.SRTMGL3.hgt/N00E109.SRTMGL3.hgt.cmr.xml",
            },
            {
                "Description": "api endpoint to retrieve temporary "
                "credentials valid for same-region "
                "direct s3 access",
                "Type": "VIEW RELATED INFORMATION",
                "URL": "https://data.lpdaac.earthdatacloud.nasa.gov/s3credentials",
            },
            {
                "Description": "Download N00E109.SRTMGL3.jpg.2.jpg",
                "Type": "GET RELATED VISUALIZATION",
                "URL": "https://data.lpdaac.earthdatacloud.nasa.gov/lp-prod-public/SRTMGL3.003/N00E109.SRTMGL3.hgt/N00E109.SRTMGL3.jpg.2.jpg",
            },
            {
                "Description": "This link provides direct download "
                "access via S3 to the granule",
                "Type": "GET RELATED VISUALIZATION",
                "URL": "s3://lp-prod-public/SRTMGL3.003/N00E109.SRTMGL3.hgt/N00E109.SRTMGL3.jpg.2.jpg",
            },
        ],
        "SpatialExtent": {
            "HorizontalSpatialDomain": {
                "Geometry": {
                    "BoundingRectangles": [
                        {
                            "EastBoundingCoordinate": 110.00083333,
                            "NorthBoundingCoordinate": 1.00083333,
                            "SouthBoundingCoordinate": -0.00083333,
                            "WestBoundingCoordinate": 108.99916667,
                        }
                    ]
                }
            }
        },
        "TemporalExtent": {
            "RangeDateTime": {
                "BeginningDateTime": "2000-02-11T00:00:00.000Z",
                "EndingDateTime": "2000-02-21T23:59:59.000Z",
            }
        },
    }

    # manually define a DataGranule for testing downloas without searching
    granule = DataGranule(
        collection={"ShortName": "SRTMGL3", "Version": "003"}, cloud_hosted=True
    )
    granule["umm"] = sample_umm
    granule["meta"] = sample_meta
    test_results = [granule]

    if len(results) == 0:
        print(f"No results from search using '{args.filename}', skipping download.")
    else:
        with Timer(msg="download from earthaccess:"):
            files = earthaccess.download(results, "./hgt_files")

        print("Downloaded files to ./hgt_files:")
        for f in files:
            print(f)
