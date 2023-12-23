#include <iostream> 
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <thread>
#include <conio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <windows.h> 
using namespace std;
string Gpath,Gpath2[100];
void sprint(string s,int time)
{
	int len=s.size();
	for(int i=0;i<len;i++)
	{
		if(s[i]=='*')
		{
			Sleep(100);
			continue;
		}
		printf("%c",s[i]);
		Sleep(time);
	}
}

bool isDirectory(const std::string& path)
{
#ifdef _WIN32
    DWORD attr = GetFileAttributes(path.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY));
#else
    DIR* dir = opendir(path.c_str());
    if (dir)
    {
        closedir(dir);
        return true;
    }
    return false;
#endif
}

void listSubdirectories(const std::string& folderPath, std::vector<std::string>& subdirectories)
{
#ifdef _WIN32
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA((folderPath + "\\*").c_str(), &findData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0)
            {
                subdirectories.push_back(findData.cFileName);
            }
        } while (FindNextFileA(hFind, &findData));
        FindClose(hFind);
    }
#else
    DIR* dir = opendir(folderPath.c_str());
    if (dir)
    {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr)
        {
            if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                subdirectories.push_back(entry->d_name);
            }
        }
        closedir(dir);
    }
#endif
}

std::string getModificationDate(const std::string& filePath)//Folder modification date
{
    std::string modificationDate;
#ifdef _WIN32
    WIN32_FIND_DATAA fileData;
    HANDLE hFile = FindFirstFileA(filePath.c_str(), &fileData);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        FILETIME ft = fileData.ftLastWriteTime;
        SYSTEMTIME st;
        FileTimeToSystemTime(&ft, &st);

        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d",
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

        modificationDate = buffer;

        FindClose(hFile);
    }
#else
    struct stat info;
    if (stat(filePath.c_str(), &info) != 0)
        return modificationDate;

    time_t t = info.st_mtime;
    struct tm* tm = localtime(&t);

    char buffer[256];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);

    modificationDate = buffer;
#endif

    return modificationDate;
}
string temp[30];
void ListFiles(const std::string& folderPath) {
    std::string searchPath = folderPath + "\\*.*";

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
	int i=0;
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            	temp[++i]=findData.cFileName;
                cout<< i << ".  " << findData.cFileName << std::endl;
            }
        } while (FindNextFileA(hFind, &findData));

        FindClose(hFind);
    }
}
void Qread()
{
	char ch;
	ch=getchar();
	while(ch!=':') ch=getchar();
	return ;
}
string GetRegistryValue(const std::string& keyPath, const std::string& valueName)
{
    HKEY hKey;
    string valueData;

    if (RegOpenKeyExA(HKEY_CURRENT_USER, keyPath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		char buffer[1024];
		DWORD dataSize = sizeof(buffer);
		if (RegGetValueA(hKey, NULL, valueName.c_str(), RRF_RT_ANY, NULL, buffer, &dataSize) == ERROR_SUCCESS) {
			valueData = buffer;
		}
		RegCloseKey(hKey);
	}
	else
	{
		printf("\n\nThis program calls 7-Zip for high compression backup, but 7-Zip has not been installed on your computer. Please download it from the official website 7-zip.org first.\n\n");
	}
    return valueData;
}
struct names{
	string real,alias;
	int x;
}name[100]; 
string rname2[20],Bpath,command,yasuo,lv;
bool prebf,ontop,choice,echos;
HWND hwnd;
struct File {
    string name;
    time_t modifiedTime;
};
int PreSolve(string s)
{
	int len=s.size(),tt=0;
	for(int i=0;i<len;++i)
	{
		if(s[i]=='$')
		{
			++tt;
			continue;
		}
		Gpath2[tt]+=s[i];
	}
	return tt;
}
void Backup(int bf,bool echo)
{
	string folderName = Bpath + "/" + name[bf].alias; // Set folder name
	// Create a folder using mkdir ()
	int result = mkdir(folderName.c_str());
	time_t now = time(0);
    tm *ltm = localtime(&now);
    string com=asctime(ltm),tmp="";
    
    for(int j=0;j<com.size();++j)
    	if(j>=11 && j<=18)
    		if(j==13 || j==16) tmp+="-";
    		else tmp+=com[j];
    tmp="["+tmp+"]"+name[bf].alias;
	if(echo) command=yasuo+" a -t7z -mx="+lv+" "+tmp+" \""+name[bf].real+"\"\\*";
	else command=yasuo+" a -t7z -mx="+lv+" "+tmp+" \""+name[bf].real+"\"\\*";
//	cout<< endl << command <<endl;//debug 
	system(command.c_str());
	if(echo) command="move "+tmp+".7z "+folderName;
	else command="move "+tmp+".7z "+folderName;
	system(command.c_str());
	return ;
}
void CreateConfig()
{
	printf("\nDo you need to create (1) general configuration or (2) fully automatic configuration\n\n");
	char ch=getch();
	string folderName,filename = "config1.ini";
	string i="1";
    ifstream file(filename);
    while(true)
    {
    	i[0]+=1;
    	filename="config"+i+".ini";
    	ifstream file(filename);
    	if(!file.is_open()) break;
	}
	if(ch=='1')
	{
		printf("\nEstablishing configuration file named %s\n",&filename[0]); 
    	ofstream newFile(filename);
    	printf("Please enter the parent folder path of the folder you are backing up (separate each folder path by $ for multiple folders): ");
		getline(cin,Gpath);
		printf("Please enter which folder you want to backup to:");
		getline(cin,Bpath);
		for(int i=0;i<=10;++i)
			Gpath2[i]="";
		int summ=PreSolve(Gpath);
        if (newFile.is_open()) {
        	newFile << "Auto:0" << endl;
        	newFile << "Backup parent folder path:" << Gpath2[0];
        	if(summ>1) newFile << '$'; 
        	for(int i=1;i<summ;++i)
        		newFile << Gpath2[i] << '$';
        	if(summ!=0) newFile << Gpath2[summ] << endl;
        	else newFile << endl;
            newFile << "Backup Storage Folder Path:" << Bpath << endl;
			string keyPath = "Software\\7-Zip"; 
			string valueName = "Path";
			string softw=GetRegistryValue(keyPath, valueName),softww="";
			for(int i=0;i<softw.size();++i)
				if(softw[i]==' ') softww+='"',softww+=' ',softww+='"';
				else softww+=softw[i];
            newFile << "Compressed software path:" << softww+"7z.exe" << endl;
            newFile << "Backup before restoring:0" << endl;
            newFile << "Toolbox Topping:0" << endl;
            newFile << "Manually select restore:0" << endl;
            newFile << "Progress Display:1" << endl;
            newFile << "Compressed Level:5" << endl;
    	}
    	printf("\nThere are the following folders in these folders:\n\n"); 
    	for(int i=0;i<=summ;++i)
    	{
    		cout << endl; 
    		std::vector<std::string> subdirectories;
			listSubdirectories(Gpath2[i], subdirectories);
		    for (const auto& folderName : subdirectories)
		    {
				std::string NGpath=Gpath2[i]+"/"+folderName;
		        std::string modificationDate = getModificationDate(NGpath);
		        std::cout << "Folder name: " << folderName << endl;
		        std::cout << "Modification date " << modificationDate << endl;
		        std::cout << "-----------" << endl;
		    }
		    Sleep(2000);
		    sprint("Next, you need to give these folders aliases that are easy for you to understand.\n\n",50);
			for (const auto& folderName : subdirectories)
		    {
		        string alias;
		        cout << "Please enter an alias for folder " << folderName << endl;
		        cin >> alias;
				newFile << folderName << endl << alias << endl;
		    }
		    newFile << "$" << endl;
		}
	    newFile << "*" << endl;
	    newFile.close();
	    sprint("The configuration file has been created!!!\n",10);
        return ;
	}
	else if(ch=='2')
	{
		ofstream newFile(filename);
		newFile << "AUTO:1" << endl;
		int configs;
		printf("Number of configuration file to be called (retrieve archive name and alias from it):\n");
		cin>>configs;
		newFile << "Use Config:" << configs << endl;
		printf("Which archive needs to be backed up:");
		cin>>configs;
		newFile << "BF:" << configs << endl;
		printf("Do you need (1) scheduled backup or (2) interval backup\n");
		ch=getch();
		if(ch=='1'){
			printf("Enter when you want to back up: 1. Please enter the month and press enter (enter 0 for each month):");
			int mon,day,hour,min;
			scanf("%d",&mon);
			printf("2. Please enter the date and press enter (enter 0 to indicate daily):");
			scanf("%d",&day);
			printf("3. Please enter the hour and then press enter (enter 0 for every hour):");
			scanf("%d",&hour);
			printf("4. Please enter the minute and then press enter:");
			scanf("%d",&min);
			newFile << "Mode:1\nTime:" << mon << " " << day << " " << hour << " " << min << endl;
		} 
		else if(ch=='2'){
			printf("Enter how many minutes you want to backup:");
			int detime;
			scanf("%d",&detime);
			newFile << "Mode:2\nTime:" << detime << endl;
		}
		else{
			printf("\nError\n");
			return ;
		}
		printf("Is the undisturbed mode enabled (0/1):");
		cin>>configs;
		newFile << "Inter:" << configs;
		sprint("The configuration file has been created!!! \n",10);
		return ;
	}
	else
	{
		printf("\n\nError\n\n");
		return ;
	}
}
void Main()
{
	string folderName;
	string filename = "config.ini";
    ifstream file(filename);
    if (!file.is_open()) {
		printf("\nEstablishing configuration file...\n"); 
    	ofstream newFile(filename);
    	printf("Please enter the parent folder path of the folder you are backing up (separate each folder path by $ for multiple folders): ");
		getline(cin,Gpath);
		printf("Please enter which folder you want to backup to:");
		getline(cin,Bpath);
		int summ=PreSolve(Gpath);
		cout<< endl << summ << endl;//debug
        if (newFile.is_open()) {
        	newFile << "The serial number of the config file used:0" << endl;
        	/*newFile << "Backup parent folder path:" << Gpath2[0] << '$';
        	for(int i=1;i<summ;++i)
        		newFile << Gpath2[i] << '$';
        	newFile << Gpath2[summ] << endl;*/
        	newFile << "Backup parent folder path:" << Gpath << endl;//new
            newFile << "Backup Storage Folder Path:" << Bpath << endl;
			string keyPath = "Software\\7-Zip"; 
			string valueName = "Path";
			string softw=GetRegistryValue(keyPath, valueName),softww="";
			for(int i=0;i<softw.size();++i)
				if(softw[i]==' ') softww+='"',softww+=' ',softww+='"';
				else softww+=softw[i];
            newFile << "Compressed software path:" << softww+"7z.exe" << endl;
            newFile << "Backup before restoring:0" << endl;
            newFile << "Toolbox Topping:0" << endl;
            newFile << "Manually select restore:0" << endl;
            newFile << "Progress Display:1" << endl;
            newFile << "Compressed Level:5" << endl;
    	}
    	printf("\nThere are the following folders in these folders:\n\n"); 
    	for(int i=0;i<=summ;++i)
    	{
    		cout << endl; 
    		std::vector<std::string> subdirectories;
			listSubdirectories(Gpath2[i], subdirectories);
		    for (const auto& folderName : subdirectories)
		    {
				std::string NGpath=Gpath2[i]+"/"+folderName;
		        std::string modificationDate = getModificationDate(NGpath);
		        std::cout << "Folder name: " << folderName << endl;
		        std::cout << "Modification date " << modificationDate << endl;
		        std::cout << "-----------" << endl;
		    }
		    Sleep(2000);
		    sprint("Next, you need to give these folders aliases that are easy for you to understand.\n\n",50);
			for (const auto& folderName : subdirectories)
		    {
		        string alias;
		        cout << "Please enter an alias for folder " << folderName << endl;
		        cin >> alias;
				newFile << folderName << endl << alias << endl;
		    }
		    newFile << "$" << endl;
		}
	    newFile << "*" << endl;
	    newFile.close();
        return ;
    }
    
    
    
    freopen("config.ini","r",stdin);
    Qread();
    int configs;
    string temps="";
    char configss[10];
	scanf("%d",&configs);
	if(configs!=0)
	{
		temps+='0';
		temps[0]=configs+'0';
		temps="config"+temps+".ini";
		for(int i=0;i<temps.size();++i)
			configss[i]=temps[i];
		freopen(configss,"r",stdin);
		Qread();
		bool auto1;
		cin>>auto1;
		if(auto1)
		{
			int mode,usecf,bfnum,ifinter;
			Qread();
			cin>>usecf;
			Qread();
			cin>>bfnum;
			Qread();
			cin>>mode;
			int bftime,month,day,hour,min;
			if(mode==1)
			{
				scanf("%d %d %d %d",&month,&day,&hour,&min);
			}
			else if(mode==2)
			{
				Qread();
				cin>>bftime;
			}
			Qread();
			cin>>ifinter;
			if(usecf==0) // 使用一般配置中的存档路径 
			{
				freopen("config.ini","r",stdin);
				string tmp;
				getline(cin,tmp);
				Qread();
			    getline(cin,temps);
			    int summ=PreSolve(temps);
			    Qread();
			    getline(cin,Bpath);
			    Qread();
			    getline(cin,yasuo);
			//    yasuo="\""+yasuo+"\""; useless
			    Qread();
			    cin>>tmp;//prebf useless
			    Qread();
			    cin>>tmp;//
			    Qread();
				cin>>tmp;//
				Qread();
				cin>>tmp;//
				Qread();
				getline(cin,lv);
			    int i=0;
//			    printf("Your folder is as follows:\n\n");
			    int ttt=0;
			    while(true)
			    {
			    	getline(cin,name[++i].real);
			    	if(name[i].real[0]=='*') break;
			    	else if(name[i].real[0]=='$')
			    	{
			    		++ttt,--i;
			    		continue;
					}
			    	name[i].real=Gpath2[ttt]+"/"+name[i].real;
			    	name[i].x=ttt;
			    	getline(cin,name[i].alias);
			    	//printf("%d. ",i);
			    	//cout<<name[i].alias<<endl;
				}
				freopen("CON","r",stdin);
				
			}
			else
			{
			    string temps="";
			    char configss[10];
				temps+='0';
				temps[0]=usecf+'0';
				temps="config"+temps+".ini";
				for(int i=0;i<temps.size();++i)
					configss[i]=temps[i];
				freopen(configss,"r",stdin);
				
				string tmp;
				getline(cin,tmp);
				Qread();
			    getline(cin,temps);
			    int summ=PreSolve(temps);
			    Qread();
			    getline(cin,Bpath);
			    Qread();
			    getline(cin,yasuo);
			//    yasuo="\""+yasuo+"\""; useless
			    Qread();
			    cin>>tmp;//prebf useless
			    Qread();
			    cin>>tmp;//
			    Qread();
				cin>>tmp;//
				Qread();
				cin>>tmp;//
				Qread();
				getline(cin,lv);
			    int i=0;
			    //printf("Your folder is as follows:\n\n");
			    int ttt=0;
			    while(true)
			    {
			    	getline(cin,name[++i].real);
			    	if(name[i].real[0]=='*') break;
			    	else if(name[i].real[0]=='$')
			    	{
			    		++ttt,--i;
			    		continue;
					}
			    	name[i].real=Gpath2[ttt]+"/"+name[i].real;
			    	name[i].x=ttt;
			    	getline(cin,name[i].alias);
			    	//printf("%d. ",i);
			    	//cout<<name[i].alias<<endl;
				}
				freopen("CON","r",stdin);
			}
			
			
			if(mode==1)
			{
				int month,day,hour,min;
				scanf("%d %d %d %d",&month,&day,&hour,&min);
				// 获取当前时间
			    std::time_t now = std::time(nullptr);
			    std::tm* local_time = std::localtime(&now);
			
			    // 设置目标时间为2023年7月1日上午6点         忘记保存啦啦 
			    std::tm target_time = {0};
			    //target_time.tm_year = 2023 - 1900; // 年份从1900年开始计算
			    if(month==0)  /*target_time.tm_mon = local_time->tm_mon*/;
			    else target_time.tm_mon = month - 1; // 月份从0开始计算
			    if(day==0) /*target_time.tm_mday = local_time->tm_mday*/;
			    else target_time.tm_mday = day;
			    if(hour==0) /*target_time.tm_hour = local_time->tm_hour*/;
			    else target_time.tm_hour = hour;
			    if(min==0) /*target_time.tm_min = local_time->tm_min*/;
			    else target_time.tm_min = min;
			
			    // 如果当前时间已经超过了目标时间，那么就不需要等待了
			    if (std::mktime(local_time) > std::mktime(&target_time)) {
			        std::cout << "现在的时间已经超过了目标时间" << std::endl;
			    } else {
			        // 计算需要等待的时间（单位：秒）
			        std::time_t wait_time = std::difftime(std::mktime(&target_time), std::mktime(local_time));
			
			        // 等待指定的时间
			        std::this_thread::sleep_for(std::chrono::seconds(wait_time));
			
			        // 在这里执行你的语句
			        std::cout << "现在是2023年7月1日上午6点整" << std::endl;
			        Backup(bfnum,false);
			    }
			} 
			else if(mode==2)
			{
				
				
//				puts("233");
				while(true)
				{
					auto now = std::chrono::steady_clock::now();
				    // 计算目标时间（例如：5秒后）
				    auto target_time = now + std::chrono::seconds(bftime);
				
				    // 计算时间差值
				    auto duration = target_time - now;
				
				    // 让线程休眠指定的时间
				    std::this_thread::sleep_for(duration);
				    Backup(bfnum,false);
				}
				
			}
			//freopen("","r",stdin) //NEWWW
		}
	}
    Qread();
    getline(cin,temps);
    int summ=PreSolve(temps);
    Qread();
    getline(cin,Bpath);
    Qread();
    getline(cin,yasuo);
//    yasuo="\""+yasuo+"\""; useless
    Qread();
    cin>>prebf;
    Qread();
    cin>>ontop;
    Qread();
	cin>>choice;
	Qread();
	cin>>echos;
	Qread();
	getline(cin,lv);
    int i=0;
    printf("Your folder is as follows:\n\n");
    int ttt=0;
    //char ch=getchar();//DEBUG because getline ...//bug why?now ok?
    while(true)
    {
    	getline(cin,name[++i].real);
    	if(name[i].real[0]=='*') break;
    	else if(name[i].real[0]=='$')
    	{
    		++ttt,--i;
    		continue;
		}
    	name[i].real=Gpath2[ttt]+"/"+name[i].real;
    	name[i].x=ttt;
    	getline(cin,name[i].alias);
    	printf("%d. ",i);
    	cout<<name[i].alias<<endl;
	}
	freopen("CON","r",stdin);
	
	while(true)
	{
		printf("Do you want to (1) back up the data, (2) restore, (3) update the folder or (4) auto backup? (5) create a config (press 1/2/3/4/5)\n");
		char ch;
		ch=getch();
		if(ch=='1')
		{
			printf("Enter the corresponding serial number to complete the corresponding data backup:");
			int bf;
			scanf("%d",&bf);
			Backup(bf,echos);
			sprint("\n\nBackup complete! ! !\n\n",40);
		}
		else if(ch=='2')
		{
			int i=0;
		    if(!choice) printf("Enter the corresponding serial number to complete the corresponding data restoration (mode: take the latest backup) : ");
		    else printf("Enter the corresponding serial number to complete the corresponding data restore (mode: manually select backup) :");
			int bf;
			scanf("%d",&bf);
			string folderPath=Bpath+"/"+name[bf].alias+"/";
			DIR* directory = opendir(folderPath.c_str());
		    if (!directory) {
		        printf("The catalog doesn't exist! You may not have backed up it yet\n");
		        return ;
		    }
		    File files;
		    if(!choice)//Look for the latest backup
		    {
			    struct dirent* entry;
			    while ((entry = readdir(directory))) {
			        string fileName = entry->d_name;
			        string filePath = folderPath + fileName;
			        struct stat fileStat;
			        if (stat(filePath.c_str(), &fileStat) != -1) {
			            if (S_ISREG(fileStat.st_mode)) { // Only regular files are processed
			                File file;
			                file.name = fileName;
			                file.modifiedTime = fileStat.st_mtime;
							if(file.modifiedTime>files.modifiedTime)
							{
								files.modifiedTime=file.modifiedTime;
								files.name=file.name;
							}
								
			            }
			        }
			    }
			    closedir(directory);
			}
			else
			{
				string folderName2 = Bpath + "/" + name[bf].alias;
				printf("There are the following backup folders\n\n");
				ListFiles(folderName2);
				printf("Enter the corresponding serial number to complete the restoration of the corresponding data:");
				int bf2;
				scanf("%d",&bf2);
				files.name=temp[bf2];
			}
		    if(prebf)
		    	Backup(bf,false);
			command=yasuo+" x "+Bpath+"/"+name[bf].alias+"/"+files.name+" -o"+name[bf].real+" -y";
			system(command.c_str());
			sprint("\n\nRestore complete! ! !\n\n",40);
		}
		else if(ch=='3')
		{
			freopen("CON","r",stdin);
        	ofstream cfile("config.ini");
        	cfile << "The serial number of the config file used:0\n";
        	cfile << "Backup parent folder path:" << Gpath2[0] << '$';
        	for(int i=1;i<summ;++i)
        		cfile << Gpath2[i] << '$';
        	cfile << Gpath2[summ] << endl;
        	cfile << "Backup Storage Folder Path:" << Bpath << endl;
			string keyPath = "Software\\7-Zip"; 
			string valueName = "Path";
            cfile << "Compressed software path:" << GetRegistryValue(keyPath, valueName)+"7z.exe" << endl;
            cfile << "Backup before restoring:" << prebf << endl;
            cfile << "Toolbox Topping:" << ontop << endl;
            cfile << "Manually select restore:" << choice << endl;
            cfile << "Progress Display:" << echos << endl;
            cfile << "Compressed Level:" << lv << endl;
        	printf("\nThere are the following folders in these folders:\n\n"); 
	    	for(int i=0;i<=summ;++i)
	    	{
	    		vector<string> subdirectories;
				listSubdirectories(Gpath2[i], subdirectories);
			    for (const auto& folderName : subdirectories)
			    {
					string NGpath=Gpath2[i]+"/"+folderName;
			        string modificationDate = getModificationDate(NGpath);
			        cout << "Folder name: " << folderName << endl;
			        cout << "Modification date" << modificationDate << endl;
			        cout << "-----------" << endl;
			    }
			    Sleep(2000);
			    sprint("Next, you need to give these folders aliases that are easy for you to understand.\n\n",50);
				for (const auto& folderName : subdirectories)
			    {
			        string alias;
			        cout << "Please enter an alias for folder " << endl << folderName;
			        cin >> alias;
					cfile << folderName << endl << alias << endl;
			    }
			    cfile << "$" << endl;
			}
		    cfile << "*" << endl;
	    	puts("\n\nUpdate folder completed\n\n");
	    	cfile.close(); 
		}
		else if(ch=='4')
		{
			printf("Enter the corresponding serial number to complete the corresponding data backup:");
			int bf,tim;
			scanf("%d",&bf);
			printf("Enter time interval(minute): ");
			scanf("%d",&tim);
			printf("Entered automatic backup mode, files will be backed up every %d minutes",tim);
			while(true)
			{
				Backup(bf,false);
				Sleep(tim*60000);
			}
		}
		else if(ch=='5')
		{
			CreateConfig();
		}
		else printf("Please Click 1/2/3/4 on the keyboard\n\n");
	}
	std::this_thread::sleep_for(std::chrono::seconds(1));
//    return ;
}

bool isRunning = true;
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
    {
        int buttonId = LOWORD(wParam);

        // Handle button click events
        switch (buttonId)
        {
	        case 1:
	            command="start "+Gpath2[0];
				system(command.c_str());
	            break;
	        case 2:
	            command="start "+Bpath;
				system(command.c_str());
	            break;
	        case 3:
	            command="start config.ini";
				system(command.c_str());
	            break;
		}
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND h=GetForegroundWindow();
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "ButtonWindowClass";
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,                          
        wc.lpszClassName,          
        "Toolbox",                  
        WS_OVERLAPPEDWINDOW,          
        CW_USEDEFAULT, CW_USEDEFAULT, 
        480, 85,              
        NULL,                     
        NULL,                     
        hInstance,
        NULL
    );
    CreateWindow("button", "Open the data folder",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        20, 10, 120, 35,
        hwnd, (HMENU)1, hInstance, NULL);

    CreateWindow("button", "Open the backup folder",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        150, 10, 120, 35,
        hwnd, (HMENU)2, hInstance, NULL);

    CreateWindow("button", "Settings",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        280, 10, 80, 35,
        hwnd, (HMENU)3, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    if(ontop) SetWindowPos(h,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);//Top the window
    std::thread MainThread(Main);
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    isRunning = false;
    MainThread.join();
    return 0;
}
