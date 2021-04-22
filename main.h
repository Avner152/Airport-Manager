#pragma once

typedef enum
{
	eAddFlight, eAddAirport, ePrintCompany, ePrintAirports,ePrintFlightOrigDest,
	eSortFlights, eSearchFlight,
	eNofOptions
} eMenuOptions;

const char* str[eNofOptions];

#define EXIT			-1
#define MANAGER_FILE_NAME "airport_authority.txt"
#define COMPANY_FILE_NAME "company.bin"



//#define AVNER_FILE_NAME "avnerAndRon.bin"


typedef unsigned char* BYTE;


BYTE compressCompany(const Company* pComp);
BYTE compressFlights(const Flight* pFlight);

void unCompressFlights(BYTE pBuffer, Flight* pComp);
void unCompressCompany(BYTE pBuffer, Company* pComp);

int writeCompressedCompany(char* txtPath, Company* pCompany);
void readCompanyFromCompressedFile(Company* company, char* companyFile);

int menu();
int initManagerAndCompany(AirportManager* pManager, Company* pCompany, char* airportFile, char* companyFile);