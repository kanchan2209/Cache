/*
Cache Simulator
Level one L1 and level two L2 cache parameters are read from file (block size, line per set and set per cache).
The 32 bit address is divided into tag bits (t), set index bits (s) and block offset bits (b)
s = log2(#sets)   b = log2(block size)  t=32-s-b
*/
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <bitset>
#include <map>
#include <vector>

using namespace std;
//access state:
#define NA 0 // no action
#define RH 1 // read hit
#define RM 2 // read miss
#define WH 3 // Write hit
#define WM 4 // write miss

struct add_seg{
	int index_int;
	string offset_str;
	string tag_str;

};
struct cacheStructure{
	string tag_array;
	int dirty_bit;
	int valid_bit;
};


struct config{
       int L1blocksize;
       int L1setsize;
       int L1size;
       int L2blocksize;
       int L2setsize;
       int L2size;
       };

 /*you can define the cache class here, or design your own data structure for L1 and L2 cache*/
class cache_struc {
public:
	string offset_str;
	string index_str;
	string tag_str;
	int s,b,t;
	int index_int;
	int l ; 
	
	
	
add_seg calc_bit(int Lsetsize,int Lsize, int Lblocksize ,string address )
     {
	add_seg segments;
	if( Lsetsize > 0)
		{
		s = (log((Lsize/Lsetsize)*(1024/Lblocksize)))/log(2);
}	
		else{
			s = 0;
			}//index bit
		//cout<<"index bit in cache:"<<s<<"\n";
		b = log(Lblocksize)/log(2);//offset bit     
		//cout<<"offset in L:"<<b<<"\n";
		t = 32- s-b;// tag bit
		//cout<<"tag bit in L:"<<t<<"\n";
       // can be called seperately if required!!!!
		segments.offset_str = address.substr(32-b,32);
		index_str = address.substr(32-b-s,32-b);
		segments.tag_str = address.substr(0,32-b-s); 
		for (l =s; l < 32; l++)
		{
		index_str = "0"+index_str;
		}
		bitset<32>index_bit = bitset<32>(index_str);
		segments.index_int = (int)(index_bit.to_ulong());
		return segments;
		   }
    

 };
     

int main(int argc, char* argv[]){
    int s_l1,s_l2,b_l1,b_l2,t_l1,t_l2;// Defining offset,tag and index bit
    cacheStructure cache_L1;
    cacheStructure cache_L2;
    /* Declaring array of maps for ways in L1, L2*/	
    std::vector<std::map<int,cacheStructure> > way_l1(32768);
    std::vector<std::map<int,cacheStructure> > way_l2(32768);
    /* Declaring maps for ways in L1, L2*/
    std::map<int, cacheStructure> map_l1; 
    std::map<int, cacheStructure> map_l2; 	

    config cacheconfig;
    ifstream cache_params;
    int i,j,x,y;
    int flag = 0;
    int flag1 = 0;
    int flag_r = 0;
    int flag_r2 =0;	
    	
    
    string dummyLine;
    cache_params.open(argv[1]);
    while(!cache_params.eof())  // read config file
    {
      cache_params>>dummyLine;
      cache_params>>cacheconfig.L1blocksize;
      cache_params>>cacheconfig.L1setsize;              
      cache_params>>cacheconfig.L1size;
      cache_params>>dummyLine;              
      cache_params>>cacheconfig.L2blocksize;           
      cache_params>>cacheconfig.L2setsize;        
      cache_params>>cacheconfig.L2size;
      }
	 int ele_count_L1 = ((cacheconfig.L1size/cacheconfig.L1setsize)*(1024/cacheconfig.L1blocksize));	
	 int ele_count_L2 = ((cacheconfig.L2size/cacheconfig.L2setsize)*(1024/cacheconfig.L2blocksize));	
      // Implement by you: 
     // initialize the hirearch cache system with those configs
      /*The 32 bit address is divided into tag bits (t), set index bits (s) and block offset bits (b)s = log2(#sets)   b = log2(block size)  t=32-s-b*/
	 /*------------------- Start intializing cache L1----------------------*/
	for ( j =0; j < cacheconfig.L1setsize; j++)//# ways
	 {	
		
		cache_L1.tag_array = "";
		cache_L1.dirty_bit = 0;
		cache_L1.valid_bit = 0;

        // initializing each node of hash map
		for (i=0;i < ele_count_L1; i++){
		//(std::pair<int,cacheStructure>(i,cache_L1);
		map_l1.insert(std::pair<int,cacheStructure>(i,cache_L1));
	 	
		}
		way_l1.at(j)=(map_l1);
		}

     /*-------------------- Start intializing cache L2------------------------*/	
	for ( x =0; x < cacheconfig.L2setsize; x++)//# ways
	 {	
		
		cache_L2.tag_array = "";
		cache_L2.dirty_bit = 0;
		cache_L2.valid_bit = 0;
	        // initializing each node of hash map
		for (y=0;y < ele_count_L2; y++){
		//(std::pair<int,cacheStructure>(i,cache_L1);
		map_l2.insert(std::pair<int,cacheStructure>(y,cache_L2));
	 	
		}
		way_l2.at(x)=(map_l2);
		}

/* ----------End Of Cache Initialization----------- */	
  
   // probably you may define a Cache class for L1 and L2, or any data structure you like
   
   
   
   
  int L1AcceState =0; // L1 access state variable, can be one of NA, RH, RM, WH, WM;
  int L2AcceState =0; // L2 access state variable, can be one of NA, RH, RM, WH, WM;
   
   
    ifstream traces;
    ofstream tracesout;
    string outname;
    outname = string(argv[2]) + ".out";
    
    traces.open(argv[2]);
    tracesout.open(outname.c_str());
    int evic_count =0;
    string line;
    string accesstype;   	 // the Read/Write access type from the memory trace;
    string xaddr;        	// the address from the memory trace store in hex;
    unsigned int addr;     // the address from the memory trace store in unsigned int;        
    bitset<32> accessaddr;// the address from the memory trace store in the bitset;


  if (traces.is_open()&&tracesout.is_open()){    
     while (getline (traces,line)){   // read mem access file and access Cache
            
            istringstream iss(line); 
            if (!(iss >> accesstype >> xaddr)) {break;}
            stringstream saddr(xaddr);
            saddr >> std::hex >> addr;
            accessaddr = bitset<32> (addr);
	    
	std::string accessaddr_str = accessaddr.to_string();
	cache_struc caches;
	add_seg result;
	add_seg result1;
	result = caches.calc_bit(cacheconfig.L1setsize,cacheconfig.L1size,cacheconfig.L1blocksize,accessaddr_str);
result1 = caches.calc_bit(cacheconfig.L2setsize,cacheconfig.L2size,cacheconfig.L2blocksize,accessaddr_str);
	cout<<"address in hex is:\n"<<accessaddr<<"\n";
	cout<<"offset returned is:\n"<<result.offset_str<<"\n";
	cout<<"tag returned is:\n"<<result.tag_str<<"\n";
	cout<<"index returned is:\n"<<result.index_int<<"\n";
		    
	if (accesstype.compare("R")==0)
              
             {    
                 //Implement by you:
                 // read access to the L1 Cache,
			j=0;flag_r =0;
			while(j < cacheconfig.L1setsize){
				cout<< " READING FROM CACHE L1\n";
				cacheStructure read_address = way_l1[j].at(result.index_int);
				string read_tA = read_address.tag_array;
				int read_vB = read_address.valid_bit;
					if ((result.tag_str==read_tA)&&(read_vB==1)){
					cout <<"READ DATA FROM ADDRESS"<<result.tag_str<< "READ DAT FROM HASH MAP"<<read_tA;
					L1AcceState = 1;
					L2AcceState = 0;
					flag_r =1;
					break;
					}
				j = j+1;
				}
		if(flag_r==0)
		{
			cout<< " READING FROM CACHE L2\n";
		 
			j=0;flag_r2 =0; 
			while(j < cacheconfig.L2setsize){
				cacheStructure read_address = way_l2[j].at(result1.index_int);
				string read_tA2 = read_address.tag_array;
				int read_vB2 = read_address.valid_bit;
					cout <<"READ DATA FROM ADDRESS"<<result.tag_str<< "READ DAT FROM HASH MAP"<<read_tA2;
					if ((result1.tag_str==read_tA2)&&(read_vB2==1)){
					cout<<"READ HIT IN L2";
					L1AcceState = 2;
					L2AcceState = 1;
					flag_r2 =1;
					// Eviction performed
					while(evic_count < cacheconfig.L2setsize )	
					{
						
					// getting value from L2 to be written to L1		
					cacheStructure etemp1 = way_l2[j].at(result1.index_int);
					string read_evictArrayl2= etemp1.tag_array;
					// 
					cacheStructure etemp2 = way_l1[evic_count].at(result1.index_int);
					string read_evictArrayl1= etemp2.tag_array;
					// writing to L1
					cache_L1.tag_array = read_evictArrayl2;
					cache_L1.dirty_bit =1;
					cache_L1.valid_bit =1;				       
					std::map<int,cacheStructure> temp3;
					temp3.insert(std::pair<int,cacheStructure>(result.index_int, cache_L1));		
					way_l1[evic_count] = temp3;
					
					// writing to L2
					cache_L2.tag_array = read_evictArrayl2;
					cache_L2.dirty_bit =1;
					cache_L2.valid_bit =1;				       
					std::map<int,cacheStructure> temp;
					temp.insert(std::pair<int,cacheStructure>(result1.index_int, cache_L2));		
					way_l2[evic_count] = temp;
					break;
					evic_count = evic_count + 1;
					if (evic_count == cacheconfig.L2setsize )
						{
							evic_count = 0; // Round Robin performed
						}
					
					}
				
					break;
					}
				 j = j+1;
			}
	if(flag_r2 ==0)
	{
		L1AcceState = 2;
		L2AcceState = 2;
	}

		}	

//cout<< "BEFORE CHECK !!!!!\n"<<read_tagArray<<"END OF CHECK";
                 //  and then L2 (if required), 
                 //  update the L1 and L2 access state variable;
             }

             else 
             {    int flagm = 0;j=0;
			while((cacheconfig.L1setsize-j-1)>=0){
				cacheStructure read_map = way_l1.at(j).at(result.index_int);
				string read_tag = read_map.tag_array;
				if (read_tag == result.tag_str)
				{
				flagm = 1;
				cout<<"flagm=1";
				break;
				}
				else{
				flagm = 0;
				cout<<"flagm =0";
				}
			    j=j+1;
			      }
                  //Implement by you:
                  // write access to the L1 Cache, 
			j =0;flag =0;
			while((j < cacheconfig.L1setsize))
			      {
				cout<<"value of j:"<<j;

				cacheStructure read_mapObject = way_l1.at(j).at(result.index_int);
				string read_tagArray = read_mapObject.tag_array;

				cout<< "BEFORE CHECK !!!!!\n"<<read_tagArray<<"END OF CHECK";

				if ((read_tagArray=="")){

					cout<<"IF CHECKED!!!!!!WRITE HIT";
					
					cache_L1.tag_array = result.tag_str;
					cache_L1.dirty_bit =1;
					cache_L1.valid_bit =1;				        
					//std::map<int,cacheStructure> temp;
					//temp[result.index_int]= cache_L1;		
					way_l1.at(j)[result.index_int]= cache_L1;

					/*cacheStructure read_mapObject1 = temp.at(result.index_int);
					string read_tagArray1 = read_mapObject1.tag_array;
					cout<< "AFTER CHECK !!!!!"<<read_tagArray1<<"END OF CHECK";*/

					flag = 1;
					break;
				       }
					
					j=j+1;		
                              }
				//update the L1 and L2 access state variable;                  
				L1AcceState = 3;
				L2AcceState = 0;                 
				/*and then L2 (if required)*/
				if(flag ==0)
						 {
					int flagm = 0;j=0;
						while((cacheconfig.L2setsize-j-1)>=0){
							cacheStructure read_map1 = way_l2.at(j).at(result1.index_int);
							string read_tag1 = read_map1.tag_array;
							if (read_tag1 == result1.tag_str)
							{
							flagm = 1;
							cout<<"flagm=1";
							break;
				}
				else{
				flagm = 0;
				cout<<"flagm =0";
				}
					j=j+1;
			      }
				cout<<"entering flag 0";
				x = 0;flag1 =0;
				while(x < cacheconfig.L2setsize)
			      {
				cacheStructure read_mapObject1 = way_l2.at(x).at(result1.index_int);
				string read_tagArray = read_mapObject1.tag_array;
				//int read_dirtbit = read_mapObject1.dirty_bit;
				cout<< "BEFORE CHECK !!!!!\n"<<read_tagArray<<"END OF CHECK";

				 if ((read_tagArray=="")&&flagm==0){

					cout<<"IF CHECKED!!!!!!WRITE HIT";
					
					cache_L2.tag_array = result1.tag_str;
					cache_L2.dirty_bit =1;
					cache_L2.valid_bit =1;						
		

				/*std::map<int,cacheStructure> temp1;
				temp1.insert(std::pair<int,cacheStructure>(result1.index_int, cache_L2));*/	
				way_l2.at(j)[result.index_int]= cache_L2;	
		

				/*cacheStructure read_mapObject2 = temp1.at(result1.index_int);
				string read_tagArray2 = read_mapObject2.tag_array;
				cout<< "AFTER CHECK !!!!!"<<read_tagArray2<<"END OF CHECK";*/

				flag1 = 1;
					break;
				       }
				x = x+ 1;
                              }
			L1AcceState = 4;
			L2AcceState = 3;  
			if (flag1==0)
			{
			L1AcceState = 4;
			L2AcceState = 4;  
			}
		}
				

              }   
                  
                 tracesout<< L1AcceState << " " << L2AcceState << endl;  // Output hit/miss results for L1 and L2 to the output file;
             
             
        }
        traces.close();
        tracesout.close(); 
    }
    else cout<< "Unable to open trace or traceout file ";


   
    
  

   
    return 0;
}
