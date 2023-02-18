#include <iostream>
#include <sqlite3.h>
#include <string>
#include <time.h>


const int UserAutogenerateCount = 1000000;

int GetRandomNumber(int Min, int Max)
{
	return  Min + rand() % (Max - Min + 1);
}

class DataBase
{

public:
	void OpenConnection()
	{
		char* zErrMsg = 0;
		int rc;

		rc = sqlite3_open(DataBaseSourse, &DataBasePointer);

		if (rc) {
			fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(DataBasePointer));
		}
	}

	void CloseConnection()
	{
		sqlite3_close(DataBasePointer);
	}

	DataBase(const char Sourse[])
	{
		DataBaseSourse = Sourse;
	}

	~DataBase()
	{
		CloseConnection();
	}

	void CreateTable()
	{

		const char* sql = "CREATE TABLE IF NOT EXISTS Person("  \
			"ID INTEGER PRIMARY KEY AUTOINCREMENT, " \
			"FullName TEXT NOT NULL," \
			"DateOfBirth DATE NOT NULL," \
			"Gender TEXT NOT NULL);";

		DoSql(sql);
	}

	void AddListOnRecords(std::string ListOfRecords[], int RecordCount)
	{
		std::string sql;

		for (int i = 0; i < UserAutogenerateCount; ++i)
		{
			sql += ListOfRecords[i];
		}

		DoSql(sql);
	}

	void AddRecord(std::string FullName, std::string DateOfBirth, std::string Gender)
	{

		std::string SqlRecord = "INSERT INTO Person(FullName,DateOfBirth,Gender) VALUES( '" + FullName + "','" + DateOfBirth + "','" + Gender + "'); ";
		DoSql(SqlRecord);
	}


	std::string GenerateUser(int Gender)
	{
		std::string RandomDate = GetRandomDate();
		int Item;
		std::string Name;
		std::string StringGender = Gender == 1 ? "Male" : "Female";
		if (Gender == 1)
		{
			Item = GetRandomNumber(0, 999);
			Name = RandomMaleNames[Item];
		}
		else
		{
			Item = GetRandomNumber(0, 901);
			Name = RandomFemaleNames[Item];
		}

		return ("INSERT INTO Person(FullName,DateOfBirth,Gender) VALUES('" + Name + "','" + RandomDate + "','" + StringGender + "');");
		//std::cout << Name << " " << RandomDate << " " << StringGender << std::endl;
		//AddRecord(Name, RandomDate, StringGender);
	}

	void GetFMale()
	{
		std::string SqlRecord = "SELECT * FROM PERSON WHERE GENDER='Male' AND FULLNAME LIKE 'F%'";
		DoSelect(SqlRecord);
	}

	void GetUserWithCalculatedField()
	{
		std::string SqlRecord = "Select FullName,DateOfBirth,Gender\n"
			", CASE\n"
			"WHEN strftime('%m', date('now')) > strftime('%m', date(DateOfBirth)) THEN strftime('%Y', date('now')) - strftime('%Y', date(DateOfBirth))\n"
			"WHEN strftime('%m', date('now')) < strftime('%m', date(DateOfBirth)) THEN strftime('%Y', date('now')) - strftime('%Y', date(DateOfBirth)) - 1\n"
			"WHEN strftime('%m', date('now')) = strftime('%m', date(DateOfBirth)) THEN\n"
			"CASE\n"
			"WHEN strftime('%d', date('now')) >= strftime('%d', date(DateOfBirth)) THEN strftime('%Y', date('now')) - strftime('%Y', date(DateOfBirth))\n"
			"ELSE\n"
			"strftime('%Y', date('now')) - strftime('%Y', date(DateOfBirth)) - 1\n"
			"END\n"

			"END AS 'Age'\n"
			"From Person\n"

			"ORDER BY FullName;";

		DoSelect(SqlRecord);
	}

private:

	void DoSelect(std::string sql)
	{
		double TimeSpent = 0.0;
		clock_t Begin = clock();


		OpenConnection();

		char* zErrMsg = 0;
		int rc;
		const char* data = "Callback function called";



		rc = sqlite3_exec(DataBasePointer, sql.c_str(), CallbackSelect, (void*)data, &zErrMsg);

		if (rc != SQLITE_OK) {
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
		else {
			fprintf(stdout, "Operation done successfully\n");
		}


		CloseConnection();

		clock_t End = clock();
		TimeSpent += (double)(End - Begin) / CLOCKS_PER_SEC;
		std::cout << "Time Spent: " << TimeSpent << std::endl;
	}

	void DoSql(std::string sql)
	{

		OpenConnection();
		sqlite3_exec(DataBasePointer, "PRAGMA journal_mode = OFF; PRAGMA synchronous = 0; PRAGMA cache_size = 1000000; PRAGMA locking_mode = EXCLUSIVE; PRAGMA temp_store = MEMORY; ", 0, 0, 0);

		int rc;
		char* zErrMsg = 0;

		rc = sqlite3_exec(DataBasePointer, sql.c_str(), Callback, 0, &zErrMsg);

		DisplayError(rc, zErrMsg);

		CloseConnection();
	}

	static int Callback(void* NotUsed, int argc, char** argv, char** azColName) {
		int i;
		for (i = 0; i < argc; i++) {
			printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		}
		printf("\n");
		return 0;
	}

	static int CallbackSelect(void* data, int argc, char** argv, char** azColName) {
		int i;
		std::cout << (stderr, "%s: ", (const char*)data);

		for (i = 0; i < argc; i++) {
			printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		}

		printf("\n");
		return 0;
	}

	std::string GetRandomDate()
	{
		std::string Year = std::to_string(GetRandomNumber(1960, 2022));
		std::string Month = std::to_string(GetRandomNumber(1, 12));
		if (Month.size() == 1)
			Month = "0" + Month;

		std::string Day = std::to_string(GetRandomNumber(1, 25));
		if (Day.size() == 1)
			Day = "0" + Day;

		std::string Date = Year + '-' + Month + '-' + Day;
		return Date;
	}

	void DisplayError(int rc, char* zErrMsg = 0)
	{
		if (rc != SQLITE_OK) {
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
		else {
			fprintf(stdout, "Operation successful\n");
		}
	}

	const char* DataBaseSourse;
	sqlite3* DataBasePointer;
	std::string RandomFemaleNames[902] = { "Aaliyah", "Abby", "Abigail", "Ada", "Adalee", "Adaline", "Adalyn", "Adalynn", "Addilyn", "Addilynn", "Addison", "Addisyn",
		"Addyson", "Adelaide", "Adele", "Adelina", "Adeline", "Adelyn", "Adelynn", "Adley", "Adriana", "Adrianna", "Adrienne", "Ailani", "Aileen", "Ainsley", "Aisha",
		"Aria", "Ariadne", "Ariah", "Ariana", "Arianna", "Ariel", "Ariella", "Arielle", "Ariya", "Ariyah", "Arlette", "Armani", "Arya", "Ashley", "Ashlyn", "Ashlynn",
		"Aspen", "Astrid", "Athena", "Aubree", "Aubrey", "Aubrie", "Aubriella", "Aubrielle", "Audrey", "August", "Aurelia", "Aurora", "Austyn", "Autumn", "Ava", "Avah",
		"Avalyn", "Avalynn", "Averi", "Averie", "Avery", "Aviana", "Avianna", "Aya", "Ayla", "Ayleen", "Aylin", "Azalea", "Azaria", "Azariah", "Bailee", "Bailey", "Barbara",
		"Baylee", "Beatrice", "Belen", "Bella", "Bellamy", "Belle", "Berkley", "Bethany", "Bexley", "Bianca", "Blair", "Blaire", "Blake", "Blakely", "Bonnie", "Braelyn",
		"Braelynn", "Braylee", "Bria", "Briana", "Brianna", "Briar", "Bridget", "Briella", "Brielle", "Brinley", "Bristol", "Brittany", "Brooke", "Brooklyn", "Brooklynn",
		"Brylee", "Brynlee", "Brynleigh", "Brynn", "Cadence", "Cali", "Callie", "Calliope", "Cameron", "Camila", "Camilla", "Camille", "Camryn", "Cara", "Carly", "Carmen",
		"Carolina", "Caroline", "Carolyn", "Carter", "Casey", "Cassandra", "Cassidy", "Cataleya", "Catalina", "Catherine", "Cecelia", "Cecilia", "Celeste", "Celia", "Celine",
		"Chana", "Chanel", "Charlee", "Charleigh", "Charley", "Charli", "Charlie", "Charlotte", "Chaya", "Chelsea", "Cheyenne", "Chloe", "Christina", "Christine", "Claire",
		"Clara", "Clare", "Clarissa", "Claudia", "Clementine", "Colette", "Collins", "Cora", "Coraline", "Corinne", "Crystal", "Cynthia", "Dahlia", "Daisy", "Dakota", "Dalary",
		"Daleyza", "Dallas", "Dana", "Dani", "Daniela", "Daniella", "Danielle", "Danna", "Daphne", "Davina", "Dayana", "Deborah", "Delaney", "Delilah", "Della", "Demi", "Destiny",
		"Diana", "Dior", "Dorothy", "Dream", "Dulce", "Dylan", "Eden", "Edith", "Egypt", "Eileen", "Elaina", "Elaine", "Eleanor", "Elena", "Eliana", "Elianna", "Elina", "Elisa",
		"Elisabeth", "Elise", "Eliza", "Elizabeth", "Ella", "Elle", "Ellen", "Elliana", "Ellianna", "Ellie", "Elliot", "Elliott", "Ellis", "Ellison", "Eloise", "Elora", "Elsa",
		"Elsie", "Elyse", "Ember", "Emberly", "Emelia", "Emely", "Emerie", "Emerson", "Emersyn", "Emery", "Emilee", "Emilia", "Emily", "Emma", "Emmaline", "Emmalyn", "Emmalynn",
		"Emmarie", "Emmeline", "Emmie", "Emmy", "Emory", "Ensley", "Erica", "Erika", "Erin", "Esme", "Esmeralda", "Esperanza", "Estella", "Estelle", "Esther", "Estrella", "Etta",
		"Eva", "Evangeline", "Eve", "Evelyn", "Evelynn", "Everlee", "Everleigh", "Everly", "Evie", "Ezra", "Faith", "Fatima", "Faye", "Felicity", "Fernanda", "Finley", "Fiona",
		"Florence", "Frances", "Francesca", "Frankie", "Freya", "Frida", "Gabriela", "Gabriella", "Gabrielle", "Galilea", "Gemma", "Genesis", "Genevieve", "Georgia", "Gia", "Giana",
		"Gianna", "Giavanna", "Giovanna", "Giselle", "Giuliana", "Gloria", "Grace", "Gracelyn", "Gracelynn", "Gracie", "Greta", "Guadalupe", "Gwen", "Gwendolyn", "Hadassah", "Hadlee",
		"Hadleigh", "Hadley", "Hailee", "Hailey", "Haisley", "Haley", "Halle", "Hallie", "Hana", "Hanna", "Hannah", "Harlee", "Harleigh", "Harley", "Harlow", "Harmoni", "Harmony",
		"Harper", "Hattie", "Haven", "Hayden", "Haylee", "Hayley", "Hazel", "Heaven", "Heavenly", "Heidi", "Helen", "Helena", "Henley", "Holland", "Holly", "Hope", "Hunter",
		"Iliana", "Imani", "India", "Ingrid", "Irene", "Iris", "Isabel", "Isabela", "Isabella", "Isabelle", "Isla", "Itzayana", "Itzel", "Ivanna", "Ivory", "Ivy", "Izabella",
		"Jacqueline", "Jada", "Jade", "Jaelyn", "Jaelynn", "Jaliyah", "Jamie", "Jana", "Jane", "Janelle", "Janessa", "Janiyah", "Jasmine", "Jaycee", "Jayda", "Jayde", "Jayden",
		"Jayla", "Jaylah", "Jaylee", "Jayleen", "Jaylene", "Jazlyn", "Jazlynn", "Jazmin", "Jazmine", "Jemma", "Jenesis", "Jenna", "Jennifer", "Jessica", "Jessie", "Jewel",
		"Jillian", "Jimena", "Joanna", "Jocelyn", "Joelle", "Johanna", "Jolene", "Jolie", "Jordan", "Jordyn", "Joselyn", "Josephine", "Josie", "Journee", "Journey", "Journi",
		"Joy", "Joyce", "Judith", "Julia", "Juliana", "Julianna", "Julie", "Juliet", "Julieta", "Juliette", "Julissa", "June", "Juniper", "Jurnee", "Justice", "Kadence", "Kaelyn",
		"Kai", "Kaia", "Kailani", "Kailey", "Kailyn", "Kairi", "Kaitlyn", "Kaiya", "Kalani", "Kali", "Kaliyah", "Kallie", "Kamila", "Kamilah", "Kamiyah", "Kamryn", "Kara", "Karen",
		"Karina", "Karla", "Karlee", "Karsyn", "Karter", "Kassidy", "Kataleya", "Katalina", "Kate", "Katelyn", "Katherine", "Kathleen", "Kathryn", "Katie", "Kaydence", "Kayla", "Kaylani",
		"Kaylee", "Kayleigh", "Kaylie", "Kaylin", "Kehlani", "Keilani", "Keily", "Keira", "Kelly", "Kelsey", "Kendall", "Kendra", "Kenia", "Kenley", "Kenna", "Kennedi", "Kennedy", "Kensley",
		"Kenzie", "Keyla", "Khaleesi", "Khloe", "Kiana", "Kiara", "Kiera", "Kimber", "Kimberly", "Kimora", "Kinley", "Kinslee", "Kinsley", "Kira", "Kora", "Kori", "Kyla", "Kylee", "Kyleigh",
		"Kylie", "Kynlee", "Kyra", "Lacey", "Laila", "Lailah", "Lainey", "Lana", "Landry", "Laney", "Lara", "Laura", "Laurel", "Lauren", "Lauryn", "Layla", "Laylah", "Lea", "Leah", "Leanna",
		"Legacy", "Leia", "Leighton", "Leila", "Leilani", "Lena", "Lennon", "Lennox", "Leona", "Leslie", "Lexi", "Lexie", "Leyla", "Lia", "Liana", "Liberty", "Lila", "Lilah", "Lilian",
		"Liliana", "Lilianna", "Lilith", "Lillian", "Lilliana", "Lillianna", "Lillie", "Lilly", "Lily", "Lilyana", "Lina", "Linda", "Lindsey", "Lisa", "Liv", "Livia", "Logan", "Lola",
		"London", "Londyn", "Lorelai", "Lorelei", "Louisa", "Louise", "Lucia", "Luciana", "Lucille", "Lucy", "Luella", "Luna", "Lyanna", "Lydia", "Lyla", "Lylah", "Lyra", "Lyric",
		"Mabel", "Maci", "Macie", "Mackenzie", "Macy", "Madalyn", "Madalynn", "Maddison", "Madeleine", "Madeline", "Madelyn", "Madelynn", "Madilyn", "Madilynn", "Madison", "Madisyn",
		"Mae", "Maeve", "Maggie", "Magnolia", "Maia", "Maisie", "Makayla", "Makenna", "Makenzie", "Malani", "Malaya", "Malaysia", "Maleah", "Malia", "Maliah", "Maliyah", "Mallory",
		"Mara", "Maren", "Margaret", "Margo", "Margot", "Maria", "Mariah", "Mariam", "Mariana", "Marianna", "Marie", "Marilyn", "Marina", "Marisol", "Marissa", "Marlee", "Marleigh",
		"Marley", "Martha", "Mary", "Maryam", "Matilda", "Mavis", "Maxine", "Maya", "Mckenna", "Mckenzie", "Mckinley", "Meadow", "Megan", "Meghan", "Meilani", "Melanie", "Melany", "Melina",
		"Melissa", "Melody", "Mercy", "Meredith", "Mia", "Miah", "Micah", "Michaela", "Michelle", "Mikaela", "Mikayla", "Mila", "Milan", "Milana", "Milani", "Milena", "Miley", "Millie", "Mina",
		"Mira", "Miracle", "Miranda", "Miriam", "Molly", "Monica", "Monroe", "Morgan", "Mya", "Myah", "Myla", "Mylah", "Myra", "Nadia", "Nala", "Nalani", "Nancy", "Naomi", "Natalia", "Natalie",
		"Nataly", "Natasha", "Nathalie", "Naya", "Nayeli", "Nevaeh", "Nia", "Nicole", "Nina", "Noa", "Noelle", "Noemi", "Nola", "Noor", "Nora", "Norah", "Nova", "Novah", "Novalee", "Nyla",
		"Nylah", "Oaklee", "Oakley", "Oaklyn", "Oaklynn", "Octavia", "Olive", "Olivia", "Opal", "Ophelia", "Paige", "Paislee", "Paisleigh", "Paisley", "Palmer", "Paloma", "Paola", "Paris",
		"Parker", "Patricia", "Paula", "Paulina", "Payton", "Pearl", "Penelope", "Penny", "Perla", "Peyton", "Phoebe", "Phoenix", "Piper", "Poppy", "Presley", "Princess", "Priscilla", "Promise",
		"Queen", "Quinn", "Rachel", "Raegan", "Raelyn", "Raelynn", "Raina", "Ramona", "Raquel", "Raven", "Rayna", "Rayne", "Reagan", "Rebecca", "Rebekah", "Reese", "Regina", "Reign", "Reina",
		"Remi", "Remington", "Remy", "Renata", "Reyna", "Rhea", "Riley", "River", "Rivka", "Robin", "Romina", "Rory", "Rosa", "Rosalee", "Rosalie", "Rosalyn", "Rose", "Roselyn", "Rosemary",
		"Rosie", "Rowan", "Royal", "Royalty", "Ruby", "Ruth", "Ryan", "Ryann", "Rylan", "Rylee", "Ryleigh", "Rylie", "Sabrina", "Sadie", "Sage", "Saige", "Salma", "Samantha", "Samara", "Samira",
		"Sandra", "Saniyah", "Saoirse", "Sara", "Sarah", "Sarai", "Sariah", "Sariyah", "Sasha", "Savanna", "Savannah", "Sawyer", "Saylor", "Scarlet", "Scarlett", "Scarlette", "Scout", "Selah",
		"Selena", "Selene", "Serena", "Serenity", "Shelby", "Shiloh", "Siena", "Sienna", "Sierra", "Simone", "Sky", "Skye", "Skyla", "Skylar", "Skyler", "Sloan", "Sloane", "Sofia", "Sophia",
		"Sophie", "Stella", "Stephanie", "Stevie", "Summer", "Sunny", "Sutton", "Sydney", "Sylvia", "Sylvie", "Talia", "Taliyah", "Tatiana", "Tatum", "Taylor", "Teagan", "Tenley", "Teresa",
		"Tessa", "Thalia", "Thea", "Tiana", "Tiffany", "Tinley", "Tinsley", "Tori", "Treasure", "Trinity", "Vada", "Valentina", "Valeria", "Valerie", "Valery", "Vanessa", "Veda", "Vera", "Veronica",
		"Victoria", "Vienna", "Violet", "Violeta", "Virginia", "Vivian", "Viviana", "Vivienne", "Waverly", "Wendy", "Whitley", "Whitney", "Willa", "Willow", "Winter", "Wren", "Wynter", "Ximena",
		"Xiomara", "Yamileth", "Yara", "Yareli", "Yaretzi", "Zahra", "Zainab", "Zaniyah", "Zara", "Zaria", "Zariah", "Zariyah", "Zaylee", "Zelda", "Zhavia", "Zoe", "Zoey", "Zoie", "Zola",
		"Zora", "Zuri" };
	std::string RandomMaleNames[1000] = { "Aaliyah","Abby","Abigail","Ada","Adalee","Adaline","Adalyn","Adalynn","Addilyn","Addilynn",
		"Addison","Addisyn","Addyson","Adelaide","Adele","Adelina","Adeline","Adelyn","Adelynn","Adley","Adriana","Adrianna","Adrienne",
		"Ailani","Aileen","Ainsley","Aisha","Aislinn","Aitana","Aiyana","Alaia","Alaina","Alana","Alani","Alanna","Alannah","Alaya","Alayah",
		"Alayna","Aleah","Aleena","Alejandra","Alena","Alessandra","Alessia","Alexa","Alexandra","Alexandria","Alexia","Alexis","Alia","Aliana",
		"Alianna","Alice","Alicia","Alina","Alisha","Alison","Alisson","Alivia","Aliya","Aliyah","Aliza","Allie","Allison","Allyson","Alma",
		"Alondra","Alora","Alyson","Alyssa","Amaia","Amalia","Amanda","Amani","Amara","Amari","Amaris","Amaya","Amayah","Amber","Amelia","Amelie",
		"Amia","Amina","Amira","Amirah","Amiya","Amiyah","Amora","Amy","Ana","Anahi","Anais","Analia","Anastasia","Anaya","Andi","Andrea","Angel",
		"Angela","Angelica","Angelina","Angie","Anika","Aniya","Aniyah","Ann","Anna","Annabel","Annabella","Annabelle","Annalee","Annalise","Anne",
		"Annie","Annika","Ansley","Antonella","Anya","April","Arabella","Arden","Arely","Ari","Aria","Ariadne","Ariah","Ariana","Arianna","Ariel",
		"Ariella","Arielle","Ariya","Ariyah","Arlette","Armani","Arya","Ashley","Ashlyn","Ashlynn","Aspen","Astrid","Athena","Aubree","Aubrey",
		"Aubrie","Aubriella","Aubrielle","Audrey","August","Aurelia","Aurora","Austyn","Autumn","Ava","Avah","Avalyn","Avalynn","Averi","Averie",
		"Avery","Aviana","Avianna","Aya","Ayla","Ayleen","Aylin","Azalea","Azaria","Azariah","Bailee","Bailey","Barbara","Baylee","Beatrice",
		"Belen","Bella","Bellamy","Belle","Berkley","Bethany","Bexley","Bianca","Blair","Blaire","Blake","Blakely","Bonnie","Braelyn","Braelynn",
		"Braylee","Bria","Briana","Brianna","Briar","Bridget","Briella","Brielle","Brinley","Bristol","Brittany","Brooke","Brooklyn","Brooklynn",
		"Brylee","Brynlee","Brynleigh","Brynn","Cadence","Cali","Callie","Calliope","Cameron","Camila","Camilla","Camille","Camryn","Cara","Carly",
		"Carmen","Carolina","Caroline","Carolyn","Carter","Casey","Cassandra","Cassidy","Cataleya","Catalina","Catherine","Cecelia","Cecilia",
		"Celeste","Celia","Celine","Chana","Chanel","Charlee","Charleigh","Charley","Charli","Charlie","Charlotte","Chaya","Chelsea","Cheyenne",
		"Chloe","Christina","Christine","Claire","Clara","Clare","Clarissa","Claudia","Clementine","Colette","Collins","Cora","Coraline",
		"Corinne","Crystal","Cynthia","Dahlia","Daisy","Dakota","Dalary","Daleyza","Dallas","Dana","Dani","Daniela","Daniella","Danielle","Danna",
		"Daphne","Davina","Dayana","Deborah","Delaney","Delilah","Della","Demi","Destiny","Diana","Dior","Dorothy","Dream","Dulce","Dylan","Eden",
		"Edith","Egypt","Eileen","Elaina","Elaine","Eleanor","Elena","Eliana","Elianna","Elina","Elisa","Elisabeth","Elise","Eliza","Elizabeth",
		"Ella","Elle","Ellen","Elliana","Ellianna","Ellie","Elliot","Elliott","Ellis","Ellison","Eloise","Elora","Elsa","Elsie","Elyse","Ember",
		"Emberly","Emelia","Emely","Emerie","Emerson","Emersyn","Emery","Emilee","Emilia","Emily","Emma","Emmaline","Emmalyn","Emmalynn","Emmarie",
		"Emmeline","Emmie","Emmy","Emory","Ensley","Erica","Erika","Erin","Esme","Esmeralda","Esperanza","Estella","Estelle","Esther","Estrella",
		"Etta","Eva","Evangeline","Eve","Evelyn","Evelynn","Everlee","Everleigh","Everly","Evie","Ezra","Faith","Fatima","Faye","Felicity","Fernanda",
		"Finley","Fiona","Florence","Frances","Francesca","Frankie","Freya","Frida","Gabriela","Gabriella","Gabrielle","Galilea","Gemma","Genesis",
		"Genevieve","Georgia","Gia","Giana","Gianna","Giavanna","Giovanna","Giselle","Giuliana","Gloria","Grace","Gracelyn","Gracelynn","Gracie",
		"Greta","Guadalupe","Gwen","Gwendolyn","Hadassah","Hadlee","Hadleigh","Hadley","Hailee","Hailey","Haisley","Haley","Halle","Hallie","Hana",
		"Hanna","Hannah","Harlee","Harleigh","Harley","Harlow","Harmoni","Harmony","Harper","Hattie","Haven","Hayden","Haylee","Hayley","Hazel",
		"Heaven","Heavenly","Heidi","Helen","Helena","Henley","Holland","Holly","Hope","Hunter","Iliana","Imani","India","Ingrid","Irene","Iris",
		"Isabel","Isabela","Isabella","Isabelle","Isla","Itzayana","Itzel","Ivanna","Ivory","Ivy","Izabella","Jacqueline","Jada","Jade","Jaelyn",
		"Jaelynn","Jaliyah","Jamie","Jana","Jane","Janelle","Janessa","Janiyah","Jasmine","Jaycee","Jayda","Jayde","Jayden","Jayla","Jaylah","Jaylee",
		"Jayleen","Jaylene","Jazlyn","Jazlynn","Jazmin","Jazmine","Jemma","Jenesis","Jenna","Jennifer","Jessica","Jessie","Jewel","Jillian","Jimena",
		"Joanna","Jocelyn","Joelle","Johanna","Jolene","Jolie","Jordan","Jordyn","Joselyn","Josephine","Josie","Journee","Journey","Journi","Joy","Joyce",
		"Judith","Julia","Juliana","Julianna","Julie","Juliet","Julieta","Juliette","Julissa","June","Juniper","Jurnee","Justice","Kadence","Kaelyn","Kai",
		"Kaia","Kailani","Kailey","Kailyn","Kairi","Kaitlyn","Kaiya","Kalani","Kali","Kaliyah","Kallie","Kamila","Kamilah","Kamiyah","Kamryn","Kara","Karen",
		"Karina","Karla","Karlee","Karsyn","Karter","Kassidy","Kataleya","Katalina","Kate","Katelyn","Katherine","Kathleen","Kathryn","Katie","Kaydence","Kayla",
		"Kaylani","Kaylee","Kayleigh","Kaylie","Kaylin","Kehlani","Keilani","Keily","Keira","Kelly","Kelsey","Kendall","Kendra","Kenia","Kenley","Kenna","Kennedi",
		"Kennedy","Kensley","Kenzie","Keyla","Khaleesi","Khloe","Kiana","Kiara","Kiera","Kimber","Kimberly","Kimora","Kinley","Kinslee","Kinsley","Kira","Kora",
		"Kori","Kyla","Kylee","Kyleigh","Kylie","Kynlee","Kyra","Lacey","Laila","Lailah","Lainey","Lana","Landry","Laney","Lara","Laura","Laurel","Lauren",
		"Lauryn","Layla","Laylah","Lea","Leah","Leanna","Legacy","Leia","Leighton","Leila","Leilani","Lena","Lennon","Lennox","Leona","Leslie","Lexi","Lexie",
		"Leyla","Lia","Liana","Liberty","Lila","Lilah","Lilian","Liliana","Lilianna","Lilith","Lillian","Lilliana","Lillianna","Lillie","Lilly","Lily","Lilyana",
		"Lina","Linda","Lindsey","Lisa","Liv","Livia","Logan","Lola","London","Londyn","Lorelai","Lorelei","Louisa","Louise","Lucia","Luciana","Lucille","Lucy",
		"Luella","Luna","Lyanna","Lydia","Lyla","Lylah","Lyra","Lyric","Mabel","Maci","Macie","Mackenzie","Macy","Madalyn","Madalynn","Maddison","Madeleine",
		"Madeline","Madelyn","Madelynn","Madilyn","Madilynn","Madison","Madisyn","Mae","Maeve","Maggie","Magnolia","Maia","Maisie","Makayla","Makenna","Makenzie",
		"Malani","Malaya","Malaysia","Maleah","Malia","Maliah","Maliyah","Mallory","Mara","Maren","Margaret","Margo","Margot","Maria","Mariah","Mariam","Mariana",
		"Marianna","Marie","Marilyn","Marina","Marisol","Marissa","Marlee","Marleigh","Marley","Martha","Mary","Maryam","Matilda","Mavis","Maxine","Maya","Mckenna",
		"Mckenzie","Mckinley","Meadow","Megan","Meghan","Meilani","Melanie","Melany","Melina","Melissa","Melody","Mercy","Meredith","Mia","Miah","Micah","Michaela",
		"Michelle","Mikaela","Mikayla","Mila","Milan","Milana","Milani","Milena","Miley","Millie","Mina","Mira","Miracle","Miranda","Miriam","Molly","Monica","Monroe",
		"Morgan","Mya","Myah","Myla","Mylah","Myra","Nadia","Nala","Nalani","Nancy","Naomi","Natalia","Natalie","Nataly","Natasha","Nathalie","Naya","Nayeli","Nevaeh",
		"Nia","Nicole","Nina","Noa","Noelle","Noemi","Nola","Noor","Nora","Norah","Nova","Novah","Novalee","Nyla","Nylah","Oaklee","Oakley","Oaklyn","Oaklynn","Octavia",
		"Olive","Olivia","Opal","Ophelia","Paige","Paislee","Paisleigh","Paisley","Palmer","Paloma","Paola","Paris","Parker","Patricia","Paula","Paulina","Payton","Pearl",
		"Penelope","Penny","Perla","Peyton","Phoebe","Phoenix","Piper","Poppy","Presley","Princess","Priscilla","Promise","Queen","Quinn","Rachel","Raegan","Raelyn",
		"Raelynn","Raina","Ramona","Raquel","Raven","Rayna","Rayne","Reagan","Rebecca","Rebekah","Reese","Regina","Reign","Reina","Remi","Remington","Remy","Renata","Reyna",
		"Rhea","Riley","River","Rivka","Robin","Romina","Rory","Rosa","Rosalee","Rosalie","Rosalyn","Rose","Roselyn","Rosemary","Rosie","Rowan","Royal","Royalty","Ruby","Ruth",
		"Ryan","Ryann","Rylan","Rylee","Ryleigh","Rylie","Sabrina","Sadie","Sage","Saige","Salma","Samantha","Samara","Samira","Sandra","Saniyah","Saoirse","Sara","Sarah",
		"Sarai","Sariah","Sariyah","Sasha","Savanna","Savannah","Sawyer","Saylor","Scarlet","Scarlett","Scarlette","Scout","Selah","Selena","Selene","Serena","Serenity",
		"Shelby","Shiloh","Siena","Sienna","Sierra","Simone","Sky","Skye","Skyla","Skylar","Skyler","Sloan","Sloane","Sofia","Sophia","Sophie","Stella","Stephanie","Stevie",
		"Summer","Sunny","Sutton","Sydney","Sylvia","Sylvie","Talia","Taliyah","Tatiana","Tatum","Taylor","Teagan","Tenley","Teresa","Tessa","Thalia","Thea","Tiana","Tiffany",
		"Tinley","Tinsley","Tori","Treasure","Trinity","Vada","Valentina","Valeria","Valerie","Valery","Vanessa","Veda","Vera","Veronica","Victoria","Vienna","Violet","Violeta",
		"Virginia","Vivian","Viviana","Vivienne","Waverly","Wendy","Whitley","Whitney","Willa","Willow","Winter","Wren","Wynter","Ximena","Xiomara","Yamileth","Yara","Yareli",
		"Yaretzi","Zahra","Zainab","Zaniyah","Zara","Zaria","Zariah","Zariyah","Zaylee","Zelda","Zhavia","Zoe","Zoey","Zoie","Zola","Zora","Zuri" };

};

void DisplayNameAndInsert(std::string Name, std::string DOB, std::string Gender, DataBase& MyDataBase)
{
	std::cout << "Name: " + Name + "DOB: " + DOB + " , Gender: " + Gender << std::endl;
	MyDataBase.AddRecord(Name, DOB, Gender);
}

void FillQuery(std::string* SQLUserAutoGenerate, DataBase& MyDataBase)
{
	for (int i = 0; i < UserAutogenerateCount; ++i)
	{
		int Gender = GetRandomNumber(0, 1);
		SQLUserAutoGenerate[i] = MyDataBase.GenerateUser(Gender);

	}
}

int main(int argc, char* argv[])
{
	int Code = 0;
	if (argv[1])
		try
	{
		Code = std::stoi(argv[1]);
	}
	catch (...)
	{
		std::cout << "Value must be number";
	}

	const char* DataBaseFileName = "Person.db";
	DataBase MyDataBase = DataBase(DataBaseFileName);

	switch (Code)
	{
	case 1: MyDataBase.CreateTable(); break;
	case 2:
		if (argc == 7)
		{
			std::string Name = argv[2];
			Name = Name + " " + argv[3];
			Name = Name + " " + argv[4];

			DisplayNameAndInsert(Name, std::string(argv[5]), std::string(argv[6]), MyDataBase);

		}
		if (argc == 5)
		{
			DisplayNameAndInsert(std::string(argv[2]), std::string(argv[3]), std::string(argv[4]), MyDataBase);
		}
	case 3: MyDataBase.GetUserWithCalculatedField(); break;
	case 4:
		std::cout << "Start generating random values" << std::endl;
		std::string* SQLUserAutoGenerate;
		SQLUserAutoGenerate = new std::string[UserAutogenerateCount];

		FillQuery(SQLUserAutoGenerate, MyDataBase);
		std::cout << "Generating is finished. Start insert" << std::endl;
		MyDataBase.AddListOnRecords(SQLUserAutoGenerate, UserAutogenerateCount);

		delete[]SQLUserAutoGenerate; break;

	case 5: MyDataBase.GetFMale(); break;


	default:
		std::cout << "Unknown parameter";
		break;
	}





	return 0;
}

