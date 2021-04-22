#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "Company.h"
#include "AirportManager.h"
#include "General.h"
#include "main.h"
#include "fileHelper.h"
#include "myMcro.h"

#define DETAIL_PRINT

int main(int argc, char* argv[])
{	
	if (argc != 3)
		return 0;
	char airportFile[MAX_STR_LEN];
	char companyFile[MAX_STR_LEN];
	sscanf(argv[1], "%s", airportFile);
	sscanf(argv[2], "%s", companyFile);


	AirportManager	manager;
	Company			company;

	initManagerAndCompany(&manager, &company, airportFile, companyFile);
	int option, i;
	int stop = 0;
	

	do
	{
		option = menu();
		switch (option)
		{
		case eAddFlight:
			if (!addFlight(&company, &manager))
				printf("Error adding flight\n");
			break;


		case eAddAirport:
			if (!addAirport(&manager))
				printf("Error adding airport\n");
			break;

		case ePrintCompany:			 
			printCompanyNameVariadic(company.name, "Hachi", "Babait", "Ba", "Olam", NULL);
			printf("Has %d flights\n", company.flightCount);

			#ifdef DETAIL_PRINT
			generalArrayFunction((void*)company.flightArr, company.flightCount, sizeof(Flight**), printFlightV);
			L_print(&company.flighDateList, printStr);
			#endif
			break;

		case ePrintAirports:
			printAirports(&manager);
			break;

		case ePrintFlightOrigDest:
			printFlightsCount(&company);
			break;

		case eSortFlights:
			sortFlight(&company);
			break;

		case eSearchFlight:
			findFlight(&company);
			break;

		case EXIT:
			printf("Bye bye\n");
			stop = 1;
			break;

		default:
			printf("Wrong option\n");
			break;
		}
	} while (!stop);

	saveManagerToFile(&manager, airportFile);
	writeCompressedCompany(companyFile, &company);

//	saveCompanyToFile(&company, companyFile);

	freeManager(&manager);
	freeCompany(&company);

	system("pause");
	return 1;
}

int menu()
{
	int option;
	printf("\n\n");
	printf("Please choose one of the following options\n");
	for (int i = 0; i < eNofOptions; i++)
		printf("%d - %s\n", i, str[i]);
	printf("%d - Quit\n", EXIT);
	scanf("%d", &option);

	//clean buffer
	char tav;
	scanf("%c", &tav);
	return option;
}

int initManagerAndCompany(AirportManager* pManager, Company* pCompany, char* airportFile, char* companyFile)
{
	int res = initManager(pManager, airportFile);
	CHECK_MSG_RETURN_0(res, "error init manager\n")
	

	if (res == FROM_FILE)
		readCompanyFromCompressedFile(pCompany, companyFile);
		//return initCompanyFromFile(pCompany,pManager, companyFile);
	else 
		initCompany(pCompany, pManager);
	
		return 1;
}

BYTE compressCompany(const Company* pComp)
{
	BYTE pBuffer = pBuffer = (BYTE)malloc(2 * sizeof(unsigned char*));

	pBuffer[0] = strlen(pComp->name)  | pComp->sortOpt << 4  | (pComp->flightCount & 0x1) << 7;
	pBuffer[1] = pComp->flightCount >> 1;

	return pBuffer;
}

BYTE compressFlights(const Flight* pFlight)
{
	BYTE pBuffer = pBuffer = (BYTE)malloc(4 * sizeof(unsigned char*));

	pBuffer[0] = pFlight->hour | (pFlight->date.day << 5);
	pBuffer[1] = (pFlight->date.day >> 3) | (pFlight->date.month << 2) | (pFlight->date.year << 6);
	pBuffer[2] = pFlight->date.year >> 2;
	pBuffer[3] = pFlight->date.year >> 10;
	return pBuffer;
}

const char* str[eNofOptions] = { "Add Flight", "Add Airport",
"PrintCompany", "Print all Airports",
	"Print flights between origin-destination",
"Sort Flights", "Search Flight" };

int writeCompressedCompany(char* txtPath, Company* pCompany)
{
	FILE* bin = fopen(txtPath, "wb");

	CHECK_NULL_MSG_CLOSE_FILE(bin, bin, "ERROR READING FILE");

	BYTE compressedCompany = compressCompany(pCompany);
	char* tempDate;
	
	puts("Saving to a bin file Compressedly...");

	if (!fwrite(compressedCompany, sizeof(char), 2, bin))
		return 0;

	if (!fwrite(pCompany->name, sizeof(char), strlen(pCompany->name) + 1, bin))
		return 0;

	// Compress flights // 
	int i;
	for (i = 0; i < pCompany->flightCount; i++)
	{
		BYTE compressedFlight = compressFlights(pCompany->flightArr[i]);
		if (!fwrite(compressedFlight, sizeof(char), 4, bin))
			return 0;

		if (!fwrite(pCompany->flightArr[i]->originCode, sizeof(char), CODE_LENGTH + 1, bin))
			return 0;
		if (!fwrite(pCompany->flightArr[i]->destCode, sizeof(char), CODE_LENGTH + 1, bin))
			return 0;
	}

	puts("Saving to a Compressed file is Done...");
	MSG_CLOSE_RETURN_0(bin, "Company bin file has been succsessfully closed");
	return 1;
}

void unCompressCompany(BYTE pBuffer, Company* pComp)
{
	pComp->sortOpt = (pBuffer[0] & 0x70) >> 4;
	pComp->flightCount = (pBuffer[1] << 1) | (pBuffer[0] >> 7);
}

void unCompressFlights(BYTE pBuffer, Flight* pFlight)
{
	pFlight->hour = pBuffer[0] & 0x1f;
	pFlight->date.day = ((pBuffer[1] & 0x3) << 3 | (pBuffer[0] >> 5) & 0x7);
	pFlight->date.month = ((pBuffer[1] >> 2) & 0xf);
	pFlight->date.year = (pBuffer[3] << 10 | pBuffer[2] << 2 | pBuffer[1] >> 6);
}

void readCompanyFromCompressedFile(Company* company, char* companyFile)
{
	L_init(&company->flighDateList);

	int i;
	BYTE myCopressed = (BYTE)malloc(sizeof(char) * 2);
	FILE* b = fopen(companyFile, "rb");

	if (fread(myCopressed, sizeof(char), 2, b) != 2)
		return 0;
	int len = myCopressed[0] & 0xf;
	len++; 	// let len be +1 //

	company->name = (char*)malloc(sizeof(char), len + 1);

	unCompressCompany(myCopressed, company);


	if (fread(company->name, sizeof(char), len, b) != len)
		return 0;

	company->flightArr = (Flight**)malloc(sizeof(Flight*) * company->flightCount);
	BYTE compressedFlights = (BYTE)malloc(sizeof(char) * 4);

		for (i = 0; i < company->flightCount; i++)
	{
		company->flightArr[i] = (Flight*)calloc(1, sizeof(Flight));
		if (!fread(compressedFlights, sizeof(char), 4, b))
			return 0;

		unCompressFlights(compressedFlights, company->flightArr[i]);

		char* srcCode = (char*)calloc(1, sizeof(char) * CODE_LENGTH + 1);
		char* destCode = (char*)calloc(1, sizeof(char) * CODE_LENGTH + 1);

		if (fread(srcCode, sizeof(char), CODE_LENGTH + 1, b) != CODE_LENGTH + 1)
			return 0;
		if (fread(destCode, sizeof(char), CODE_LENGTH + 1, b) != CODE_LENGTH + 1)
			return 0;

		strcpy(company->flightArr[i]->originCode, srcCode);
		strcpy(company->flightArr[i]->destCode, destCode);

	}


		MSG_CLOSE_RETURN_0(b, "file has been succsessfully read");

}