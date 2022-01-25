"""download.py: Gets car accident data from all czech regions."""

__author__      = "Libor Dvoracek"
__email__       = "xdvora2t@stud.fit.vutbr.cz"

from bs4 import BeautifulSoup
import urllib.request
import numpy as np
import requests
import zipfile
import pickle
import gzip
import csv
import os

regions_dict = {
  "PHA": "00",
  "STC": "01",
  "JHC": "02",
  "PLK": "03",
  "ULK": "04",
  "HKK": "05",
  "JHM": "06",
  "MSK": "07",
  "OLK": "14",
  "ZLK": "15",
  "VYS": "16",
  "PAK": "17",
  "LBK": "18",
  "KVK": "19"
}

file_list = [
    'datagis2016.zip',
    'datagis-rok-2017.zip',
    'datagis-rok-2018.zip',
    'datagis-rok-2019.zip',
    'datagis-09-2020.zip'
]

schema = ['ID', 'druh_pozemni_komunikace', 'cislo_pozemni_komunikace', 'datum',
            'den', 'cas', 'druh_nehody', 'druh_srazky', 'pevne_prekazky',
            'smrt_hmotna_skoda', 'zavineni',
            'alkohol_vinik', 'hlavni_pricina', 'usmrcen', 'zraneni_tezka',
            'zraneni_lehka', 'celkova_skoda', 'druh_povrchu', 'stav_povrchu',
            'stav_komunikace', 'povetrnostni_podminky', 'viditelnost', 'rozhled',
            'deleni_komunikace', 'situovani_nehody', 'rizeni_provozu',
            'mistni_prednost', 'specificka_mista', 'smeerove_pomery',
            'pocet_vozidel', 'misto_nehody', 'druh_krizujici_komunikace',
            'druh_vozidla', 'znacka_vozidla', 'rok_vyroby',
            'charakteristika_vozidla', 'smyk', 'stav_po_nehode', 'unik_hmot',
            'zpusob_vyprosteni', 'smer_jizdy', 'skoda_na_vozidle',
            'kateg_ridice', 'stav_ridice', 'ovlivneni_ridice', 'a',
            'b', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'n', 'o', 'p',
            'q', 'r', 's', 't', 'lokalita', 'zkratka_regionu'
]


class DataDownloader():

    def __init__(
            self,
            url="https://ehw.fit.vutbr.cz/izv/",
            folder="data",
            cache_filename="data_{}.pkl.gz"
            ):
        """Initializer - contains optional parameters."""

        self.url = url
        self.folder = folder
        self.cache_filename = cache_filename
        self.cache = {}

    def download_data(self, input_file):
        """Downloads all zip files from "url" and save to self.folder."""

        # check data folder existence
        if not os.path.exists(self.folder):
            os.makedirs(self.folder)

        # download page
        head = {'User-Agent': 'Mozilla/5.0 (Macintosh; \
                Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 \
                (KHTML, like Gecko) Version/14.0 Safari/605.1.15'}
        resp = requests.get(self.url, headers=head).text
        soup = BeautifulSoup(resp, "html.parser")

        # extract all *.zip file paths and download files
        for file in soup.findAll("a", {"class":"btn btn-sm btn-primary"}):
                file = file['href']
                if file == input_file:
                    path = os.getcwd() + "/" + file
                    link = self.url + file
                    urllib.request.urlretrieve(link, path)

    def validity_check(self, input_list):
        """Check and erase invalid characters."""

        for item in input_list:
            # column "l" /invalid character ";"/
            if len(item) > 65:
                item[55] = 'NaN'
                item.pop(56)
            # indalid time -> NaN
            if int(item[5]) > 2500:
                item[5] = 'NaN'
            # invalid characters in column 35
            if str(item[34]) == 'XX':
                item[34] = '-1'
            # invalid characters in columns 46 - 51
            if ":" in str(item[45]):
                item[45] = "-1"
            if ":" in str(item[46]):
                item[46] = "-1"
            if ":" in str(item[47]):
                item[47] = "-1"
            if ":" in str(item[48]):
                item[48] = "-1"
            if ":" in str(item[49]):
                item[49] = "-1"
            if ":" in str(item[50]):
                item[50] = "-1"
        return input_list

    def get_column_type(self, input_list):
        """Changes columns datatype."""

        for i in range(65):
            if i == 0:
                input_list[i] = input_list[i].astype(np.uint64, copy=False)
            elif i == 16 or i == 60 or i == 61:
                input_list[i] = input_list[i].astype(np.uint32, copy=False)
            elif i >= 6 and i <= 15:
                input_list[i] = input_list[i].astype(np.uint16, copy=False)
            elif i == 4:
                input_list[i] = input_list[i].astype(np.uint16, copy=False)
            elif i >= 41 and i <= 44:
                input_list[i] = input_list[i].astype(np.uint16, copy=False)
            elif i >= 17 and i <= 40:
                input_list[i] = input_list[i].astype(np.uint8, copy=False)
            elif i == 1 or i == 63:
                input_list[i] = input_list[i].astype(np.uint8, copy=False)
            elif i >= 45 and i <=50:
                input_list[i] = input_list[i].astype('float', copy=False)

        input_list[3] = input_list[3].astype('datetime64', copy=False)
        # time to float because int cant have nan values
        input_list[5] = input_list[5].astype('float', copy=False)
        return input_list

    def parse_region_data(self, region, region_number):
        """Returns tuple(list[str], list[np.ndarray])."""

        input_string = ""
        list_ndarrays = []                         # output list
        region_file = region_number + ".csv"

        # open and parse each file from specified legion
        for file in file_list:

            path_to_file = self.folder + "/" +file

            # check if file exists
            if not os.path.exists(path_to_file):
                print("downloading {}".format(path_to_file))
                self.download_data(path_to_file)

            # open and parse
            with zipfile.ZipFile(self.folder + "/" + file, 'r') as csvs:
                with csvs.open(region_file, 'r') as infile:

                    # input means all lines from one file
                    input = infile.read().decode('ISO-8859-2')
                    input = input.replace(';;',';-1;')
                    input = input.replace(';;',';-1;')
                    input = input.replace(',','.')
                    input = input.replace('""','"-1"')
                    input = input.replace('"','')
                    input_string = input_string + input
        parsed = csv.reader(input_string.splitlines(), delimiter=';')
        parsed_list = list(parsed)

        # add last column (region abbreviation)
        for line in parsed_list:
            line.append(region)

        parsed_list = self.validity_check(parsed_list)

        # get rid off column l double ;;
        i = -1
        for item in parsed_list:
            i =+ 1
            if len(item) > 65:
                item.pop(56)

        # concatenate each column into list
        for i in range(65):
            temp = []
            for p in parsed_list:
                temp.append(p[i])
            " ".join(temp)
            temp = temp[:-1]                       #remove trailing  space
            tmparr = np.array(temp)
            list_ndarrays.append(tmparr)           #make list of 1d arrays

        list_ndarrays = self.get_column_type(list_ndarrays)
        # insert into cache file
        c_file = gzip.open(self.cache_filename.format(region_number), 'wb', compresslevel=1)
        c_file.write(pickle.dumps(list_ndarrays,0))
        c_file.close()

        # insert into memory
        self.cache[region] = list_ndarrays
        return schema, list_ndarrays

    def get_list(self, regions=None):
        """Concatenates data from all input regions,
        returns tuple(list[str], list[np.ndarray])."""

        # init output list of np.ndarrays
        output_list = None

        if regions is None:
            #work with all regions
            regions = regions_dict.keys()

        for region in regions:
            # check region validity
            try:
                region_number = regions_dict[region]
            except:
                print("Invalid region code. {}".format(region))
                exit(1)
            # get data from memory
            if region in self.cache:
                list_ndarrays = self.cache[region]
            else:
                # get data from cache file
                if os.path.isfile("data_{}.pkl.gz".format(region_number)):
                    input_file = gzip.GzipFile(self.cache_filename.format(region_number), 'rb')
                    data = input_file.read()
                    object = pickle.loads(data)
                    list_ndarrays = object
                    input_file.close()
                else:
                    #parse data from original archive
                    _,list_ndarrays= self.parse_region_data(region, region_number)

            # concatenate all input regions
            if output_list is None:
                output_list = list_ndarrays
            else:
                output_list = np.concatenate([output_list,list_ndarrays], axis=1)
        print(output_list[1].dtype)
        return schema, output_list

def print_stats(regions, data):
    print("Sloupce tabulky: {}".format(schema))
    print("Zastoupene regiony: {}".format(regions))
    print("Pocet zaznamu: {}".format(len(data[1])))

if __name__ == "__main__":
    default_regions = ['KVK', 'PAK', 'PLK']
    data = DataDownloader()
    _, output = data.get_list(default_regions)
    print_stats(default_regions, output)
