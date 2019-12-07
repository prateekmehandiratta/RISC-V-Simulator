
#include <bits/stdc++.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
using namespace std;



int full_a_cache_index_tracker = 0;

static unsigned char memory[4000];
 int read_word(unsigned char *mem, unsigned int address){
            mem+=address;
            int *p = (int*)mem;
            return *p;
           //// return 56;
    }    


struct cache_block{
    int tag_bits;
    unsigned char* mem; //memory of a block
    int dirty_bits;
    int valid_bits;
    int update_bits;
    
   
};


int binaryToDecimal(string s) {
    // your code goes here
    int value = 0;
    int indexCounter = 0;
    for(int i = s.length()-1; i >= 0; i--)
    {
 
      if(s[i] == '1')
        {
        value += pow(2, indexCounter);
        }
    indexCounter++;
    }
    return value;
}

int string_int(string s)
{
    int i = 0;
    for (int j=0 ;j< s.length() ;j++)
    {
        if (s[j] >= '0' && s[j] <= '9') {
            i = i * 10 + (s[j] - '0');
        }
    }
    return i;
    
}

int binaryToDecimal(long long n) 
{ 
    long long  num = n; 
    int dec_value = 0; 

    // Initializing base vacache_blocklue to 1, i.e 2^0 
    int base = 1; 

    long long  temp = num; 
    while (temp) { 
        long long  last_digit = temp % 10; 
        temp = temp / 10; 

        dec_value += last_digit * base; 

        base = base * 2; 
    } 

    return dec_value; 
} 
long long binConv(string binNum)
{
    long long decimal = 0;
    int size = binNum.size();
    
    for (int counter = 0; counter <size; counter++)
    {
        if (binNum[counter] == '1')
            decimal = 10*decimal + 1;

        else
            decimal = 10*decimal;
                                                //decimal=10*decimal+binNum[counter]-'0';
    }
    return decimal;
}



string HexToBin(string hexdec) 
{ 

    long int i = 2; 
    string s;

    while (hexdec[i]) { 

        switch (hexdec[i]) { 
        case '0': 
            s+="0000"; 
            break; 
        case '1': 
            s+="0001"; 
            break; 
        case '2': 
            s+="0010"; 
            break; 
        case '3': 
            s+="0011"; 
            break; 
        case '4': 
            s+="0100"; 
            break; 
        case '5': 
            s+="0101"; 
            break; 
        case '6': 
            s+="0110"; 
            break; 
        case '7': 
            s+="0111"; 
            break; 
        case '8': 
            s+="1000"; 
            break; 
        case '9': 
            s+="1001"; 
            break; 
        case 'A': 
        case 'a': 
            s+="1010"; 
            break; 
        case 'B': 
        case 'b': 
            s+="1011"; 
            break; 
        case 'C': 
        case 'c': 
            s+="1100"; 
            break; 
        case 'D': 
        case 'd': 
            s+="1101"; 
            break; 
        case 'E': 
        case 'e': 
            s+="1110"; 
            break; 
        case 'F': 
        case 'f': 
            s+="1111"; 
            break; 
        default: 
             return "0";
            //s+="\nInvalid hexadecimal digit %c", 
            //  hexdec[i]); 
        } 
        i++; 
    } 
    return s;
}
 
class base_class_cache
{
public:
    base_class_cache(){}
    virtual string access(string instruction,char b_w){}
    virtual void write_back_cache(string instruction ){}
    virtual void print(){};
} ;



class direct_m_cache : public base_class_cache
{
public:
            int cache_size; //in words
            int block_size; //in words
            int lines;

            int block_offset_digits;
            int indexing_digits;

            cache_block* cache_body; //array of cache blocks

            

    //constructor
    direct_m_cache(int c_size,int  b_size){

        cache_size = c_size;
        block_size = b_size;
        lines = c_size / b_size;

        cache_body = (cache_block*)malloc(lines*sizeof(cache_block));

        reset_cache();      //load default values
        calculate_cache_parameters();   //calculate indexing and block offset digits
        string access(string instruction, char byteORword);  //access the data in cache and return the response
        void write_back_chache_dm(string instruction ,int output_str_value); //in case of miss, load data in cache
        void write_back_cache(string instruction);
        int fetch_missed_data(string instruction); //helps write_back by extracting data
        //used to read a word from memory
    }
    //initialize cache with default values
    void reset_cache(){

            int i;
            for(i = 0; i< lines; i++){
                cache_body[i].tag_bits = -1;
                cache_body[i].dirty_bits = -1;
                cache_body[i].valid_bits = -1;
                cache_body[i].update_bits = -1;
                cache_body[i].mem = (unsigned char*)malloc(block_size*sizeof(int));
                *(cache_body[i].mem) = -1;        
               
                     }
    }

//sets indexing_digits and block_offset_digits 

    void calculate_cache_parameters(){
        indexing_digits = log2(lines);
        block_offset_digits = log2(block_size);
    }
    
    //reads a word from memory
    /*int read_word(unsigned char *mem, unsigned int address){
            mem+=address;
            int *p = (int*)mem;
            return *p;
    }    */


    string access(string instruction, char byteORword){

        string output = "0000"; //output string

        //string bin_address = HexToBin(instruction);
        string byte_offset = instruction.substr(instruction.size()-2,2);
        string block_offset = instruction.substr(instruction.size()-block_offset_digits-2,block_offset_digits);
        string cache_indexing = instruction.substr(instruction.size()-block_offset_digits-indexing_digits-2,indexing_digits);
        string tag = instruction.substr(0,instruction.size()-block_offset_digits-indexing_digits-2);

        int int_block_offset = binaryToDecimal(block_offset);
        int int_cache_indexing = binaryToDecimal(cache_indexing);
        int int_byte_offset = binaryToDecimal(byte_offset);
        int int_tag = binaryToDecimal(tag);

        
        int miss = 0, cold_miss = 0, conflict_miss = 0, capacity_miss = 0;

        if(*cache_body[int_cache_indexing].mem == 255 || cache_body[int_cache_indexing].tag_bits != int_tag){
            miss++;
            if(*cache_body[int_cache_indexing].mem == 255){
            cold_miss++;
            }
            else if(cache_body[int_cache_indexing].tag_bits != int_tag){
            conflict_miss++;
            }
           // write_back_chache(instruction, fetch_missed_data(instruction));
           if(miss>0){output[0] = '1';}
           if(cold_miss>0){output[1] = '1';}
           if(conflict_miss>0){output[2] = '1';}
           if(capacity_miss>0){output[3] = '1';}
           int int_address = binaryToDecimal(instruction);
           int read_result = 0;
            if(byteORword == 'b' || 'B'){
                read_result = memory[int_address];
            }
            else if(byteORword == 1){
                read_result = read_word(memory,int_address);

            }
            stringstream ss;
            ss << read_result;
            string str1 = ss.str();
            output += str1;
            return output;
           //cout<<"output"<<output<<endl;
           write_back_cache_dm(instruction);
           return output;
            
        } // miss

        else{   
            output[0] = '0';
            output[1] = '0';
            output[2] = '0';
            output[3] = '0';
            int read_result = 0;

            //string bin_address = HexToBin(instruction);
            int int_address = binaryToDecimal(instruction);
            if(byteORword == 'b' || 'B'){
                read_result = memory[int_address];
            }
            else if(byteORword == 'W'||'w'){
                read_result = read_word(memory,int_address);

            }
            stringstream ss;
            ss<<read_result;
            string str1 = ss.str();
            output += str1;
            return output;
        }
    }

    /* int fetch_missed_data(string instruction){
         //string bin_instr = HexToBin(instruction);
         int int_bin_instr = binaryToDecimal(instruction);
         int*p = (int*)malloc(block_size*sizeof(int));
         unsigned char* m = memory;
         m+=int_bin_instr;
         p = (int*)(m);
         return *p;
        }
        */

    void write_back_cache_dm(string instruction){

            int int_bin_instr = binaryToDecimal(instruction);
            int*p = (int*)malloc(block_size*sizeof(int));
            unsigned char* m = memory;
            m+=int_bin_instr;
            p = (int*)(m);

            string cache_indexing = instruction.substr(instruction.size()-block_offset_digits-indexing_digits-2,indexing_digits);
            int int_cache_indexing = binaryToDecimal(cache_indexing);
            *(cache_body[int_cache_indexing].mem) = *p;
            string tag = instruction.substr(0,instruction.size()-block_offset_digits-indexing_digits-2);
            int int_tag = binaryToDecimal(tag);
            cache_body[int_cache_indexing].tag_bits = int_tag;
            
    }
    void write_back_cache(string ins){
    
    }

    
};


class full_a_cache : public base_class_cache
{
public:
            int cache_size; //in words
            int block_size; //in words
            int ways;

            int block_offset_digits;
            //int indexing_digits;

            cache_block* cache_body; //array of cache blocks

            

    //constructor
    full_a_cache(int c_size,int  b_size){

        cache_size = c_size;
        block_size = b_size;
        ways = c_size / b_size;

        cache_body = (cache_block*)malloc(ways*sizeof(cache_block));

        reset_cache();      //load default values
        calculate_cache_parameters();   //calculate indexing and block offset digits
        string access(string instruction, char byteORword);  //access the data in cache and return the response
        void write_back_chache_fa(string instruction); //in case of miss, load data in cache
        void write_back_cache(string inst);
        int fetch_missed_data(string instruction); //helps write_back by extracting data
        //cout<<"Inside cache constructor"<<endl;
        //cout<<"cache_size"<<cache_size<<endl;
        //cout<<"block_size"<<block_size<<endl;
        //cout<<"ways"<<ways<<endl;
        //cout<<"block offset digits"<<block_offset_digits<<endl;
    }
    //initialize cache with default values
    void reset_cache(){

            int i;
            for(i = 0; i< ways; i++){
                cache_body[i].tag_bits = -1;
                cache_body[i].dirty_bits = -1;
                cache_body[i].valid_bits = -1;
                cache_body[i].update_bits = -1;
                cache_body[i].mem = (unsigned char*)malloc(block_size*sizeof(int));
                *(cache_body[i].mem) = -1;        
               
                     }
    }
        

//sets indexing_digits and block_offset_digits 

    void calculate_cache_parameters(){
        //indexing_digits = log2(lines);
        block_offset_digits = log2(block_size);
    }
    

    string access(string instruction, char byteORword){
        //cout<<"cache body"<<((cache_body+2)->mem)<<endl;
        string output = "0000"; //output string

        //string bin_address = HexToBin(instruction);
        string byte_offset = instruction.substr(instruction.size()-2,2);
        string block_offset = instruction.substr(instruction.size()-block_offset_digits-2,block_offset_digits);
        //string cache_indexing = instruction.substr(instruction.size()-block_offset_digits-indexing_digits-2,indexing_digits);
        string tag = instruction.substr(0,instruction.size()-block_offset_digits-2);

        int int_block_offset = binaryToDecimal(block_offset);
        //int int_cache_indexing = binaryToDecimal(cache_indexing);
        int int_byte_offset = binaryToDecimal(byte_offset);
        int int_tag = binaryToDecimal(tag);
        //cout << "tag"<<int_tag<<endl;
        //cout<<"int_tag"<<int_tag<<endl;
        //cout<<"int_block_offset::"<<block_offset<<endl;

        //loop for searching
        int i; int value_found = 0,negative_val_found = 0;
        for(i = 0; i< ways;i++){
            int a = (int)*(cache_body[i].mem);
            //cout <<"a"<<a<<endl;
            if(cache_body[i].tag_bits == int_tag){value_found++;};
            if(a == 255){negative_val_found++;};
        }

        int miss = 0, cold_miss = 0, conflict_miss = 0, capacity_miss = 0;

        if(value_found == 0){
            miss++;
            //cout<<"miss::"<<miss<<endl;
            if(negative_val_found > 0){
            cold_miss++;
            }
            else if(negative_val_found == 0){
            capacity_miss++;
            }
            //cout<<"miss::"<<miss<<endl;
            //conflict miss will not happen in fully associative
           // write_back_chache(instruction, fetch_missed_data(instruction));
           //output = miss;
           //cout << "output[0]" << output<<endl;
           
           //cout<< output<<endl;
           if(miss>0){output[0] = '1';}
           if(cold_miss>0){output[1] = '1';}
           if(conflict_miss>0){output[2] = '1';}
           if(capacity_miss>0){output[3] = '1';}
           int int_address = binaryToDecimal(instruction);
           int read_result = 0;
            if(byteORword == 'b' || 'B'){
                read_result = memory[int_address];
            }
            else if(byteORword == 1){
                read_result = read_word(memory,int_address);

            }
            stringstream ss;
            ss << read_result;
            string str1 = ss.str();
            output += str1;
            return output;
           //cout<<"output"<<output<<endl;
           write_back_cache_fa(instruction);
           return output;
            
        } // miss

        else{   
            output[0] = '0';
            output[1] = '0';
            output[2] = '0';
            output[3] = '0';
            int read_result = 0;

            //string bin_address = HexToBin(instruction);
            int int_address = binaryToDecimal(instruction);
            if(byteORword == 'b' || 'B'){
                read_result = memory[int_address];
            }
            else if(byteORword == 'w'||'W'){
                read_result = read_word(memory,int_address);

            }
            stringstream ss;
            ss << read_result;
            string str1 = ss.str();
            output += str1;
            return output;
        }
    }

     /*int fetch_missed_data(string instruction){
         //string bin_instr = HexToBin(instruction);
         int int_bin_instr = binaryToDecimal(instruction);
         int*p = (int*)malloc(block_size*sizeof(int));
         unsigned char* m = memory;
         m+=int_bin_instr;
         p = (int*)(m);
         return *p;
        }*/

    void write_back_cache_fa(string instruction){
            //string cache_indexing = instruction.substr(instruction.size()-block_offset_digits-indexing_digits-2,indexing_digits);
            //int int_cache_indexing = binaryToDecimal(cache_indexing);
            
            int int_bin_instr = binaryToDecimal(instruction);
            int*p = (int*)malloc(block_size*sizeof(int));
            unsigned char* m = memory;
            m+=int_bin_instr;
            p = (int*)(m);
            
            
            if(full_a_cache_index_tracker == 128){
                full_a_cache_index_tracker = 0;
            }
            *(cache_body[full_a_cache_index_tracker].mem) = *p;
            string tag = instruction.substr(0,instruction.size()-block_offset_digits-2);
            int int_tag = binaryToDecimal(tag);
            cache_body[full_a_cache_index_tracker].tag_bits = int_tag;
            full_a_cache_index_tracker++;            
    }
    
    void write_back_cache(string inst){}

    
};


class set_assc_cache :public base_class_cache
{
    int row,set_offset,set_size,tag,byte_offset,block_offset ,block_size ,sa_way;
    std::vector<string>* v;
    string ** cache_memory;

    public:
     set_assc_cache(int cache_size, int block_size, int sa_ways)
     {   
                                                                //block and cache size should be multiple of 4 
         row = cache_size / block_size;                         // sa_ways multiple of 2
         set_size = row / sa_ways;
         this->block_size = block_size;
         this->sa_way =sa_ways;
                  
                                              //string cache_memory[row][block_size+1]  ;
        cache_memory = new string* [row];
        for (int i = 0; i < row; ++i)
        {
            cache_memory[i]= new string [block_size+1];
        }

        for (int i = 0; i < row; ++i)
        {
            for (int j = 0; j < block_size+1; ++j)
            {
                cache_memory[i][j] = "000000000";
            }
        }
 


        v= new vector<string> [sa_ways];
         cout<< sa_ways;
        for (int j = 0; j < sa_ways; ++j)
        {
            for (int i = 0; i < set_size; ++i)
            {
                int count =0;
                string s ="";
                while (count< 33)
                {
                    s = s + "0";                // innitialise by 32+1 bits
                    count ++ ;
                }
                v[j].push_back(s);
            }
                
        }
         
        block_offset =0 , set_offset=0, tag=0 , byte_offset = 2 ;

        while(block_size>0){
            block_size /= 2;
            block_offset ++;
        }


        while(set_size>0){
            set_size /= 2;
            set_offset ++;
        } 
                 set_offset--;
                                           //1st  bit each v[i] is replacement bit
                                                            //last two bits are byte offset
                                                            //block offset 
        tag = 32 - byte_offset - block_offset  ;

     }

    string access(string instruction ,char b_w)
    {
        /*for (int i = 0; i < row; ++i)
        {
            for (int j = 0; j < block_size+1; ++j)
            {
                cout << cache_memory[i][j] <<"  ";
            }
            cout << endl;
        }*/
        
        string bin_block_number , s_to_return ="0000";
        int cold=0;
           for (int i = set_offset ; i >0 ; --i)
           {
              bin_block_number += instruction[32-i];
           }

           int block_number = binConv(bin_block_number);
           block_number =binaryToDecimal(block_number);

           for (int i = 0; i < sa_way; ++i)
           {
              string a="0"+ instruction;
              string b="1"+ instruction;

              if(v[i][block_number]== a|| v[i][block_number]== b )
              {
                   v[i][block_number][0] ='1';

                   for (int k = 0; k < sa_way; ++k)     //to implement LRU
                   {
                        if(k == i){                          //for cold miss
                            continue;
                        }
                        v[k][block_number][0] = '0';
                   }                    

                    s_to_return [0] = '0';
                    if(b_w == 'W' || b_w == 'w' )
                        return (s_to_return + cache_memory[i * set_offset + block_number][1]);  
                    else if(b_w == 'B' || b_w == 'b')
                    {
                        for (int k = 0; k < 2; ++k)
                        {
                            s_to_return += cache_memory[i * set_offset + block_number][1][k];
                        }
                        return s_to_return;
                    }

              }
              else if(v[i][block_number]=="000000000000000000000000000000000")   //for cold miss
                        cold++;   
            
             }
           s_to_return [0] ='1';

           if (cold>0)
             s_to_return [1] = '1';
           else
             s_to_return [2] = '1';

            return s_to_return;
    }

    void write_back_cache(string instruction)
    {
             string bin_block_number , s_to_return ="0000";
             int cold=0;    
             stringstream ss;  

               for (int i = set_offset ; i >0 ; --i)
               {
                  bin_block_number += instruction[32-i];
               }

               
              int  block_number = binConv(bin_block_number);
                block_number =binaryToDecimal(block_number);

              int current_way=0;
               for (int i = 0; i < sa_way; ++i)
               {    cout<<v[i][block_number]<<endl;

                    if(v[i][block_number]=="000000000000000000000000000000000"){  //for cold miss
                        cold++;
                        current_way = i;
                        break;
                    }
               }

              cout<<"===="<<sa_way<<" "<<cold;

               if (cold>0)
               {
                    v[current_way][block_number] = "1"+instruction;
                    cache_memory[current_way * set_offset + block_number][0]=   instruction;
                    int dec = binConv(instruction);
                     dec =binaryToDecimal(dec);


                    for (int i = 1; i < block_size; ++i)
                    {
                            ss << read_word(memory,dec);
                            cache_memory[current_way * set_offset + block_number][i] = ss.str() ;  
                            dec += 4;                         
                    }       
               }
               else


               {
                   for (int i = 0; i < sa_way; ++i)
                   {
                        if(v[i][block_number][0]=='1'){  //for cold miss
                            current_way = i;
                            break;
                        }
                   } 

                    v[current_way][block_number] = "1"+instruction;
                    cache_memory[current_way * set_offset + block_number][0]=   instruction;
                    int dec = binConv(instruction);
                   dec =binaryToDecimal(dec);


                    for (int i = 1; i < block_size; ++i)
                    {
                            ss << read_word(memory,dec);
                            cache_memory[current_way * set_offset + block_number][i] = ss.str();  
                            dec += 4;                         
                    }   

                   for (int i = 0; i < sa_way; ++i)     //to implemrny LRU
                   {
                        if(i == current_way){  //for cold miss
                            continue;
                        }
                        v[i][block_number][0] = '0';
                   }                

               }

    }

    void print()
    {
        for (int i = 0; i < row; ++i)
        {
            for (int j = 0; j < block_size+1; ++j)
            {
                cout << cache_memory[i][j] << " ";
            }
            cout << endl;
        }
    }

};





class cache_class
{
     private:
        int cache_size,block_size,sa_ways;     //input
        string c_type;                      //input

        int n_hits,n_access,n_misses,cold_misses,conflict_misses,capacity_misses;  //output
        base_class_cache*  inst_cache;
        base_class_cache*  data_cache;

      void intiate()
        {
          cout<<endl<<"Enter cache_size:    ";
          cin>> cache_size;
          cout<<endl<<"Enter block_size:    ";
          cin>> block_size;
          cout<<endl<<"Enter chache implement type: ";
          cin>> c_type;
          if (c_type=="SA"|| c_type=="sa")
          {
            cout<<endl<<"Enter Number of ways of set associative cache: ";
            cin>> sa_ways;
             inst_cache = new set_assc_cache(cache_size, block_size, sa_ways);
             data_cache = new set_assc_cache (cache_size, block_size, sa_ways);
          }
          else if( c_type=="DM"|| c_type=="dm")
          {
              inst_cache = new direct_m_cache(cache_size, block_size);
              data_cache = new direct_m_cache(cache_size, block_size);
          }
          else if( c_type=="FA"|| c_type=="fa")
          {
              inst_cache = new full_a_cache(cache_size, block_size);
              data_cache = new full_a_cache(cache_size, block_size);
          }
        }   


      public:

        cache_class()
        {
            intiate();
            cache_size=0;block_size=0;sa_ways=0;
            n_hits=0;n_access=0;n_misses=0;cold_misses=0;conflict_misses=0;capacity_misses=0;
            
        }

        int access(int instruc ,char i_d_cache , char byte_word)   // i_d_cache means which cahche to access data or instruction
        {

            string s ,output_str_value;
            stringstream ss;    
            
            bitset<32> b32;
            b32 = instruc;
            string instruction = b32.to_string<char,std::string::traits_type,std::string::allocator_type>();  
            
            if(i_d_cache == 'I'||i_d_cache == 'i')
                 s = inst_cache->access(instruction ,'W'); 
                                                                  //s is 4 bits + instructions value bits
            if(i_d_cache == 'D'||i_d_cache == 'd')
            {
                if(byte_word == 'w'||byte_word == 'W')
                    s = data_cache->access(instruction , 'W');   // s is 4 bits + data value bits
            }
           
           n_access ++;

           if (s[0] == '1')  //miss
           {
             n_misses ++;

             if (s[1] == '1')
                cold_misses++;
             else if (s[2] == '1')
                conflict_misses++; 
             else if (s[3] == '1')
                capacity_misses++;  

           //    output_str_value = memory_access(instruction);

            if(i_d_cache == 'I'||i_d_cache == 'i')
            {
                int dec = binConv(instruction);
                    dec =binaryToDecimal(dec);
                ss << read_word(memory, dec);
                output_str_value = ss.str();
                inst_cache->write_back_cache(instruction);
            }

            if(i_d_cache == 'D'||i_d_cache == 'd')
            {
                if(byte_word == 'w'||byte_word == 'W')
                {
                    int dec = binConv(instruction);
                    dec =binaryToDecimal(dec);
                    ss << read_word(memory , dec);
                    output_str_value = ss.str();
                    data_cache->write_back_cache(instruction);
                }
                if(byte_word == 'b' || byte_word == 'B')
                {
                    int dec = binConv(instruction);
                    dec =binaryToDecimal(dec);
                    ss << memory[dec];
                    output_str_value = ss.str() ;
                    data_cache->write_back_cache(instruction);
                }

            }

           }
           else
           {
             n_hits ++;
             for (int i = 4; i < s.size(); ++i)
             {
                output_str_value += s[i];
             }
           }

           return string_int(output_str_value);
        }

       void inst_cache_print()
       {
           inst_cache->print();
       }
        void data_cache_print()
       {
           data_cache ->print();
       }
       
        void print()
        {
            cout<<endl<<"Number of access          : "<<n_access;
            cout<<endl<<"Number of misses          : "<<n_misses;
            cout<<endl<<"Number of hits            : "<<n_hits;
            cout<<endl<<"Number of cold_misses     : "<<cold_misses;
            cout<<endl<<"Number of conflict_misses : "<<conflict_misses;
            cout<<endl<<"Number of capacity_misses : "<<capacity_misses;
            cout<<endl;
        }

};



int mem_call(int address,char i_d,char b_w );


cache_class c;


//Register file
static int reg[32];

//memory


//CLOCK CYCLES
static int clock_cycles = 0;

//Stack pointer
static unsigned int sp = 4000;

//Instruction register & program counter & interstage registers;
static int IR,PC,Ra,Rb,imm,Rd,Rz, Control_line, temp_PC, Ry, Rm, Rm_temp;

// it is used to set the reset values reset all registers and memory content to 0
void reset()
{
    for(int i=0;i<32;i++)
        reg[i]=0x00000000;
    reg[2] = sp;        //Stack pointer
    for(int i=0;i<4000;i++)
        memory[i]=0x00;
}

void write_word(unsigned char *mem, unsigned int address, unsigned int data)
{
    mem+=address;
    int *p = (int*)mem;
    *p = data;
}

//load_program_memory reads the input memory, and pupulates the instruction memory
void load_program_memory()
{
    FILE *fp;
    unsigned int address,word;
    fp = fopen("machine_code.mc", "r");
    if(fp == NULL)
    {
        printf("Error opening input mem file\n");
        exit(1);
    }
    while(fscanf(fp, "%x %x", &address, &word) != EOF)
    {
        if(word== 0xffffffff)
            continue;
        write_word(memory, address, word);  
    } 
        
    fclose(fp);
    for(int i=0;i<32;i++)
        reg[i] = 0x0;
    reg[2]=sp;          // initialising registers with zero
}

//writes the data memory in "data_out.mem" file
void write_data_memory() 
{

    FILE * fp;
    unsigned int i;
    fp = fopen("data_out.mem", "w");
    if(fp == NULL) 
    {
        printf("Error opening dataout.mem file for writing\n");
        return;
    }
    for(int i=0;i<32;i++)
    {
        fprintf(fp,"reg%d:%d\n",i,reg[i]);
    }
    for(i=0; i < 4000; i = i+4)
    {
        //int word = read_word(memory,i);
        //fprintf(fp, "%x %x\n", i, read_word(memory, i));
        fprintf(fp, "%x %x\n", i, read_word(memory,i));
        /*fprintf(fp,"%x ",i);
        fprintf(fp,"%x",memory[i]);
        fprintf(fp,"%x",memory[i+1]);
        fprintf(fp,"%x",memory[i+2]);
        fprintf(fp,"%x\n",memory[i+3]);*/
    }   
        
    fclose(fp);
}

void fetch()
{
    unsigned char *mem = memory;
    int *p = (int*)(mem+PC);
    IR = 0x00000000;
    //int Id = 0x00000000;
    //IR = *p;
    IR = mem_call(PC ,'I','W');
    if(IR==285212655)
        IR = -1*IR;
    //Id = mem_call(PC ,'I','W');
   // cout<<*p <<" "<<IR<<"[["<<Id;
    PC+=4;
}

/************ DECODE **************************/
unsigned int opcode(unsigned int word)
{
    unsigned int temp = 0x0000007F;
    temp = temp & word;
    return temp;
}

unsigned int rd(unsigned int word)
{
    unsigned int temp = 0x00000F80;
    temp = temp &  word;
    temp = temp/128;
    return temp;
}

unsigned int func3(unsigned int word)
{
    unsigned int temp = 0x00007000;
    temp = temp & word;
    temp = temp/4096;
    return temp;
}

unsigned int rs1(unsigned int word)
{
    unsigned int temp = 0x000F8000;
    temp = temp & word;
    temp = temp/32768;
    return temp;
}

unsigned int rs2(unsigned int word)
{
    unsigned int temp = 0x01F00000;
    temp = temp & word;
    temp = temp/1048576;
    return temp;
}

unsigned int func7(unsigned int word)
{
    unsigned int temp = 0xFE000000;
    temp = temp & word;
    temp = temp/33554432;
    return temp;
}

int imm_11_0 (unsigned int word)
{
    int temp = 0xFFF00000;
    temp = temp & word;
    temp = temp/1048576;
    return temp;
}

int imm_4_0 (unsigned int word)
{
    return rd(word);
}

int imm_11_5 (unsigned int word)
{
    return func7(word);
}

int imm_4_1_11 (unsigned int word)
{
    return rd(word);
}

int imm_12_10_5 (unsigned int word)
{
    return func7(word);
}

int imm_31_12 (unsigned int word)
{
    int temp = 0xFFFFF000;
    temp = temp&word;
    temp = temp/4096;
    return temp;
}

int imm_20_10_1_11_19_12 (unsigned int word)
{
    return imm_31_12(word);
}
/**********************************************************/
void extract_r()
{
    cout << "rs1:" << rs1(IR) << " rs2:" << rs2(IR) << " rd:" << rd(IR) << endl; 
    Ra = reg[rs1(IR)];
    Rb = reg[rs2(IR)];
    Rd = rd(IR);
    int opc = opcode(IR);
    int f3 = func3(IR);
    int f7 = func7(IR);     
    if(opc==51)
    {
        if(f3== 0 && f7==0)
            Control_line = 1;           //add inst

        else if(f3== 0 && f7==32)
            Control_line = 2;           //sub inst

        else if(f3==1 && f7==0) 
            Control_line = 3;           //sll inst

        else if(f3==2 && f7==0) 
            Control_line = 4;   //slt inst

        else if(f3==3 && f7==0) 
            Control_line = 5;       //sltu inst

        else if(f3==4 && f7==0)         
            Control_line = 6;           //xor inst

        else if(f3==5 && f7==0)
            Control_line = 7;           //srl inst

        else if(f3==5 && f7==32)
            Control_line = 8;           //sra inst

        else if(f3==6 && f7==0)
            Control_line = 9;           //or inst

        else if(f3==7 && f7==0)
            Control_line = 10;          //and inst

        else if(f3==0 && f7==1)
            Control_line = 11;          //mul inst

        else if(f3==1 && f7==1)
            Control_line = 12;          //mulh inst

        else if(f3==2 && f7==1)
            Control_line = 13;          //mulhsu inst
        
        else if(f3==3 && f7==1)
            Control_line = 14;          //mulhu inst
        
        else if(f3==4 && f7==1)
            Control_line = 15;          //div inst
        
        else if(f3==5 && f7==1)
            Control_line = 16;          //divu inst
        
        else if(f3==6 && f7==1)
            Control_line = 17;          //rem inst
        
        else if(f3==7 && f7==1)
            Control_line = 18;          //remu inst
    }

    else if(opc==59)
    {
        if(f3==0 && f7==0)          //addw inst
            Control_line = 21;
        
        else if(f3==0 && f7==32)            //subw inst
            Control_line = 22;

        else if(f3==1 && f7==0)         //sllw inst
            Control_line = 23;
        
        else if(f3==5 && f7==0)         //srlw inst
            Control_line = 24;

        else if(f3==5 && f7==48)
            Control_line = 25;          //sraw inst
        
        else if(f3==0 && f7==1 )
            Control_line = 26;              //mulw inst
        
        else if(f3==4 && f7==1 )
            Control_line = 27;                  //divw inst
        
        else if(f3==7 && f7==1)
            Control_line = 19;          //remw inst
        
        else if(f3==7 && f7==1)
            Control_line = 20;          //remuw inst    
    }       
}

void extract_i()
{
    Ra = reg[rs1(IR)];
    imm = imm_11_0(IR);
    int temp = 0x00000800;
    temp = temp & imm;
    if(temp==0x800)
    {
        temp = 0xfffff000;
        imm = imm | temp;
    }
    Rd = rd(IR);
    int opc = opcode(IR);
    int f3 = func3(IR);

    if(opc==3)
    {   
        if(f3==0)
            Control_line = 28;          //lb inst
        
        else if(f3==1)
            Control_line = 29;          //lh inst       
        
        else if(f3==2)
            Control_line = 30;          //lw inst       
        
        else if(f3==3)
            Control_line = 31;          //ld inst       

        else if(f3==4)
            Control_line = 32;          //lbu inst      

        else if(f3==5)
            Control_line = 33;          //lhu inst      

        else if(f3==6)
            Control_line = 34;          //lwu inst  
    }

    else if(opc==19)
    {   
        if(f3==0)
            Control_line = 35;          //addi inst
        
        else if(f3==1)
            Control_line = 36;          //slli inst DOUBTFUL        

        else if(f3==2)
            Control_line = 37;      //slti inst     
        
        else if(f3==3)
            Control_line = 38;      //sltiu inst                    
        
        else if(f3==4)
            Control_line = 39;          //xori inst     
        
        else if(f3==5)
        {       
            int f7 = func7(IR);
            if(f7==0)
                Control_line = 40;          //srli inst     DOUBTFULL           
        
            else if(f7==32)
                Control_line = 41;          //srai inst DOUBTFULL
        }
        
        else if(f3==6)
            Control_line = 42;          //ori   inst            
        
        else if(f3==7)
            Control_line = 43;      //andi inst         
    }

    else if(opc==27)
    {
        if(f3==0)
            Control_line = 44;      //addiw inst                
        
        else if(f3==1)
        {
            int f7 = func7(IR);
            if(f7==0)
                Control_line = 45;      //slliw inst
        }
        
        else if(f3==5)
        {
            int f7 = func7(IR);
            if(f7==0)
                Control_line = 46;          //srliw inst
    
            else if(f7==32)
                Control_line = 47;      //sraiw inst
        }
        
    }

    else if(opc==103)
    {
        if(f3==0)
            Control_line = 48;      //jalr inst
    }
}

void extract_s()
{
    Ra = reg[rs1(IR)];
    Rb = reg[rs2(IR)];
    imm = (imm_11_5(IR) * 32) + imm_4_0(IR) ;
    int temp = 0x00000800;
    temp = temp & imm;
    if(temp==0x800)
    {
        temp = 0xfffff000;
        imm = imm | temp;
    }
    int f3 = func3(IR);

    if(f3==0)
        Control_line = 49;      //sb inst

    else if(f3==1)
        Control_line = 50;          //sh inst
    
    else if(f3==2)
        Control_line = 51;          //sw inst   
}

void extract_sb()
{
    Ra = reg[rs1(IR)];
    Rb = reg[rs2(IR)];
    cout<<"Ra:"<<Ra<<" Rb:"<<Rb<<endl;
    unsigned int temp = imm_12_10_5(IR);
    imm = temp& 0b1000000;
    imm = imm * 32;
    temp = temp & 0b0111111;
    temp = temp * 16;
    imm = imm | temp;
    temp = imm_4_1_11(IR);
    temp = temp & 0b11110;
    temp = temp/2;
    imm = imm | temp;
    temp = imm_4_1_11(IR);
    temp = temp& 0b00001;
    temp = temp * 1024;
    imm = imm | temp;
    temp = 0x00000800;      // for negative immediate
    temp = temp & imm;
    if(temp==0x800)
    {
        temp = 0xfffff000;
        imm = imm | temp;
    }
    imm = imm << 1;
    cout<<"extracted imm:"<<imm<<endl;
    printf("extracted imm:%x\n",imm);

    int f3 = func3(IR);
    if(f3==0)
        Control_line = 52;      //beq
    
    else if(f3==1)
        Control_line = 53;      //bne
    
    else if(f3==4)
        Control_line = 54;      //blt
    
    else if(f3==5)
        Control_line = 55;      //bge
    
    else if(f3==6)
        Control_line = 56;      //bltu
    
    else if(f3==7)
        Control_line = 57;      //bgeu
}

void extract_u()
{
    imm = imm_31_12(IR);
    int temp = 0x00080000;
    temp = temp & imm;
    if(temp == 0x80000)
    {
        temp = 0xfff00000;
        imm = imm | temp;
    }
    Rd = rd(IR);
    int opc = opcode(IR);

    if(opc==23)
        Control_line = 58;      //auipc
    
    else if(opc == 55)
        Control_line = 59;      //lui 
}

void extract_uj()
{
    Rd = rd(IR);
    unsigned int temp = imm_20_10_1_11_19_12(IR);
    imm = temp & 0b00000000000011111111;
    imm = imm * 2048;
    temp = temp & 0b01111111111000000000;
    temp = temp/512;
    imm = imm | temp;
    temp = imm_20_10_1_11_19_12(IR);
    temp = temp & 0b00000000000100000000;
    temp = temp * 4;
    imm = imm | temp;
    temp = imm_20_10_1_11_19_12(IR);
    temp = temp & 0b10000000000000000000;
    imm = temp | imm;
    temp = 0x00080000;
    temp = temp & imm;
    if(temp == 0x80000)
    {
        temp = 0xfff00000;
        imm = imm | temp;
    }
    imm = imm << 1;

    Control_line = 60;      //jal
} 

void decode()
{
    int opc = opcode(IR);
    int f3 = func3(IR);
    if( opc==3 || opc==19 || opc==27 || opc == 103)
        extract_i();
    else if( opc==51 || opc==59)
        extract_r();
    else if(opc==35)            //In Reference sheet sd is given I Format.    ?????
        extract_s();
    else if(opc==99)
        extract_sb();
    else if(opc==23 || opc ==55)
        extract_u();
    else if(opc==111)
        extract_uj();
}

void execute()
{
    if(Control_line==1)
    {
        Rz = Ra + Rb;       //add
    }
    else if(Control_line==2)
    {
        Rz = Ra - Rb;       //sub
    }
    else if(Control_line==3)
    {
        Rz = Ra << Rb;      //sll
    }
    else if(Control_line==4)
    {
        Rz=(Ra<Rb)?1:0;     //slt
    }
    else if(Control_line==5)
    {
        unsigned int Ra_temp,Rb_temp;
        Ra_temp = Ra;
        Rb_temp = Rb;
        Rz=(Ra_temp<Rb_temp)?1:0;       //sltu
    }
    else if(Control_line==6)
    {
        Rz=Ra^Rb;       //xor
    }
    else if(Control_line==7)
    {
                Rz=(int)((unsigned int)Ra >> Rb);       //srl
    }
    else if(Control_line==8)
    {

        Rz=Ra>>Rb;      //sra
    }
    else if(Control_line==9)
    {
        Rz=Ra|Rb;       //or
    }
    else if(Control_line==10)
    {
        Rz=Ra&Rb;       //and
    }
    else if(Control_line==11)
    {
        Rz=Ra*Rb;       //mul
    }
    else if(Control_line==12)
    {
                //mulh
    }
    else if(Control_line==13)
    {
                //mulhsu
    }
    else if(Control_line==14)
    {
                //mulhu
    }
    else if(Control_line==15)
    {
        Rz = Ra/Rb;     //div
    }
    else if(Control_line==16)
    {
        unsigned int Ra_temp,Rb_temp;
        Ra_temp = Ra;
        Rb_temp = Rb;
        Rz = Ra_temp/Rb_temp;       //divu
    }
    else if(Control_line==17)
    {
        Rz = Ra%Rb;     //rem
    }
    else if(Control_line==18)
    {
        unsigned int Ra_temp,Rb_temp;
        Ra_temp = Ra;
        Rb_temp = Rb;       //remu
        Rz = Ra_temp%Rb_temp;
    }
    else if(Control_line==21)
    {
        Rz = Ra + Rb;       //addw
    }
    else if(Control_line==22)
    {
        Rz = Ra - Rb;       //subw
    }
    else if(Control_line==19)
    {
        Rz = Ra%Rb;     //remw
    }
    else if(Control_line==20)
    {
        unsigned int Ra_temp,Rb_temp;
        Ra_temp = Ra;
        Rb_temp = Rb;       
        Rz = Ra_temp%Rb_temp;       //remuw
    }
    else if(Control_line == 23)
    {
        Rz=Ra<<Rb;          //sllw inst 
    }   
    else if(Control_line == 24)
    {
                Rz=(int)((unsigned int)Ra >> Rb);       
            //srlw inst
    }
    else if(Control_line == 25)
    {
        Rz=Ra>>Rb;              //sraw inst
    }
    else if(Control_line == 26)
    {
        Rz = Ra * Rb;                   //mulw inst
    }
    else if(Control_line == 27)
    {
        Rz = Ra/Rb;                 //divw inst
    }
    else if(Control_line == 28)
    {
        Rz = Ra + imm;              //lb inst
    }
    else if(Control_line == 29)
    {
        Rz = Ra + imm;              //lh inst
    }
    else if(Control_line == 30)
    {
        Rz = Ra + imm;
cout << "Rz:"<<Rz << endl;              //lw inst
    }
    else if(Control_line==31)
    {
                            //ld inst
    }
    else if(Control_line==32)
    {
        Rz=imm+Ra;      //lbu
    }
    else if(Control_line==33)
    {
        Rz=imm+Ra;  //lhu
    }
    else if(Control_line==34)
    {
        Rz=imm+Ra;  //lwu
    }
    else if(Control_line==35)
    {
        Rz= Ra + imm;           // addi
    }
    else if(Control_line==36)
    {
        Rz=Ra<<imm;             //slli
    }
    else if(Control_line==37)
    {
        Rz=(Ra<imm)?1:0;            //slti
    }
    else if(Control_line==38)
    {
        unsigned int Ra_temp = Ra;
        unsigned int imm_temp = imm & 0x00000fff;
        Rz=(Ra_temp<imm_temp)?1:0;          //sltiu
    }
    else if(Control_line==39)
    {
        Rz=Ra^imm;              //xori  
    }
    else if(Control_line==40)
    {
        Rz=(int)((unsigned int)Ra >> imm);              //srli inst     DOUBTFULL
    }
    else if(Control_line==41)
    {
        Rz=Ra>>imm;                 //srai inst DOUBTFULL
    }
    else if(Control_line==42)
    {
        Rz=Ra|imm;              //ori
    }
    else if(Control_line==43)
    {
        Rz=Ra & imm;            //andi
    }
    else if(Control_line==44)
    {
        Rz=Ra + imm;            //addi
    }
    else if(Control_line==45)
    {
        Rz=(int)((unsigned int)Ra >> imm);          //srliw 
    }
    else if(Control_line==46)
    {
        Rz=Ra<<imm;             //slliw
    }
    else if(Control_line==47)
    {
        Rz=Ra>>imm;                 //sraiw inst
    }
    else if(Control_line==48)
    {   
        Rz= Ra+imm;             //jalr inst
    }
    else if(Control_line==49)
    {
        Rz = Ra + imm;                  
        Rm = Rb;                    //sb inst
    }
    else if(Control_line==50)
    {
        Rz = Ra + imm;                  
        Rm = Rb;                    //sh inst
    }
    else if(Control_line==51)
    {
        Rz = Ra + imm;                  //sw inst
        Rm = Rb;
    }
    else if(Control_line==52)
    {
        //cout<<"YES---------------\n";
        if(Ra==Rb)
        {
            Rz=1;
            //PC=PC+imm;
        }                   //beq
        else
        {
            Rz=0;
            //PC=PC;
        }
    }
    else if(Control_line==53)
    {
        if(Ra!=Rb)
        {
            Rz=1;
            //PC=PC+imm;
        }                   //bne
        else
        {
            Rz=0;
            //PC=PC;
        }
    }
    else if(Control_line==54)
    {
        if(Ra<Rb)
        {
            Rz=1;
            //PC=PC+imm;
        }                   //blt
        else
        {
            Rz=0;
            //PC=PC;
        }
    }
    else if(Control_line==55)
    {
        if(Ra>=Rb)
        {
            Rz=1;
            //PC=PC+imm;
        }                   //bge                   
        else
        {
            Rz=0;
            //PC=PC;
        }
    }   
    else if(Control_line==56)
    {
        unsigned int Ra_temp = Ra, Rb_temp = Rb;
        if(Ra_temp<Rb_temp)
        {
            Rz=1;
            //PC=PC+imm;
        }                   //bltu
        else
        {
            Rz=0;
            //PC=PC;
        }
    }   
    else if(Control_line==57)
    {
        unsigned int Ra_temp = Ra, Rb_temp = Rb;
        if(Ra_temp>=Rb_temp)
        {
            Rz=1;
            //PC=PC+imm;
        }                   //bgeu                  
        else
        {
            Rz=0;
            //PC=PC;
        }
    }
    else if(Control_line==58)
    {
        Rz = imm;
        cout <<"Rz:"<< Rz << endl;          //auipc
    }
    else if(Control_line==59)
    {
        Rz=imm<<12;                 //lui
    }
    else if(Control_line==60)
    {
                            //jal
    }
}

void memory_access()
{
    if(Control_line==28)
    {
        //Ry=memory[Rz];
      cout<<"Rz"<< Rz;
      Ry=mem_call(Rz,  'D' ,'W' );
    int temp = 0x00000080;
    temp = temp & Ry;
    if(temp==0x80)
    {
        temp = 0xffffff00;
        Ry = Ry | temp;
    }
    }

    else if(Control_line==29)
    {
        //Ry=256*memory[Rz]+memory[Rz+1];
    /*int temp = 0x00008000;
    temp = temp & Ry;
    if(temp==0x8000)
    {
        temp = 0xffff0000;
        Ry = Ry | temp;
    }*/
        //int temp=read_word(memory,Rz);
        cout<<"Rz"<< Rz;
        int temp=mem_call(Rz,  'D' ,'W' );

        Ry=temp & 0x0000ffff;
        printf("Ry:%x\n",Ry);
        int temp2 = 0x00008000;
    temp2 = temp2 & Ry;
    printf("temp2:%x\n",temp2);
    if(temp2==0x8000)
    {
        temp2 = 0xffff0000;
        Ry = Ry | temp2;
    }
    printf("Ry:%x\n",Ry);
    //cout<<"Ry:"<<Ry<<endl;

    }

    else if(Control_line==30)
    {
       //Ry=pow(256,3)*memory[Rz]+pow(256,2)*memory[Rz+1]+pow(256,1)*memory[Rz+2]+memory[Rz+3];
    // Ry = read_word(memory,Rz); 
            cout<<"Rz"<< Rz;
           Ry=mem_call(Rz,  'D' ,'B' );

    cout << "Ry:"<<Ry << " " << endl;
    }

    /*else if(Control_line==31)
    {
        //Not to be used(requires 64 bit)
        //Ry=memory[Rz+7]+pow(256,1)*memory[Rz+6]+pow(256,2)*memory[Rz+5]+pow(256,3)*memory[Rz+4]+pow(256,4)*memory[Rz+3]+pow(256,5)*memory[Rz+2]+pow(256,6)*memory[Rz+1]+power(256,7)*memory[Rz];
    }*/

    else if(Control_line==32)
    {
       // Ry=memory[Rz];
         cout<<"Rz"<< Rz;
         Ry=mem_call(Rz,  'D' ,'B' );
    }

    else if(Control_line==33)
    {
        //int temp=read_word(memory,Rz);
        int temp=mem_call(Rz,  'D' ,'W' );
        Ry=temp & 0x0000ffff;
        //Ry=256*memory[Rz]+memory[Rz+1];
    }

    else if(Control_line==34)
    {
       //Ry=pow(256,3)*memory[Rz]+pow(256,2)*memory[Rz+1]+pow(256,1)*memory[Rz+2]+memory[Rz+3];
        //Ry = read_word(memory,Rz);

         Ry=mem_call(Rz,  'D' ,'B' );

    }

    else if(Control_line==49)
    {
        memory[Rz]=Rm;
    }

    else if(Control_line==50)
    {
        memory[Rz+1]=Rm%256;
        Rm_temp=Rm/256;
        memory[Rz]=Rm_temp%256;
    }

    else if(Control_line==51)
    {
        Rm_temp=Rm;
        /*memory[Rz+3]=Rm_temp%256;
        Rm_temp=Rm_temp/256;
        memory[Rz+2]=Rm_temp%256;
        Rm_temp=Rm_temp/256;
        memory[Rz+1]=Rm_temp%256;
        Rm_temp=Rm_temp/256;
        memory[Rz]=Rm_temp%256;*/
    write_word(memory,Rz,Rm);
    }

    else if(Control_line==48)
    {
        temp_PC=PC;
        PC=Rz;
    }

    else if(Control_line==60)
    {
        temp_PC=PC;
        PC -= 4;
        PC+=imm;
    }

    else if(Control_line==52||Control_line==53||Control_line==54||Control_line==55||Control_line==56||Control_line==57)
    {
        cout<<"Rz:"<<Rz<<endl;
        if(Rz==1)
        {
            cout<<"PC jump"<<" imm:"<<imm<<endl;
            PC -= 4;
            PC+=imm;
        }
    }

    else
    {
        Ry=Rz;
    }
}

// Registry update
void writeback()
{
    if(Control_line==49||Control_line==50||Control_line==51||Control_line==52||Control_line==53||Control_line==54||Control_line==55||Control_line==56||Control_line==57)
    {
        return;
    }

    else if(Control_line==48||Control_line==60)
    {
        reg[Rd]=temp_PC;
    }
    else
    {
        reg[Rd] = Ry;
    }
    return;
}

int main()
{
    PC = 0x0;reg[2]=sp;
    load_program_memory();
    
    while(1)
    {
        fetch();
        cout<<"*";
        if(IR == 0xEF000011)
            break;
        decode();
        //cout<<"Control_line:"<<Control_line<<endl;
        //cout << "Ra:" << Ra << " Rb:" << Rb << endl;
        execute();
        memory_access();
        writeback();
        //cout << reg[10]<<" Ry:"<<Ry<<" PC:"<<PC<<" Rz:"<<Rz<<endl;
        clock_cycles++; 
    }
    write_data_memory();
    cout << "No. of clock cycles: " << clock_cycles << endl;
     
    /* c.access(2004, 'D' ,'W');
     c.access(2004, 'I' ,'W');
     c.access(2004, 'D' ,'W');
     mem_call(2004,  'D' ,'W'  );
     mem_call(2004,  'I' ,'W' );
     c.inst_cache_print();*/

    c.print();


}


int mem_call(int address,char i_d,char b_w )
{

         c.access(address , i_d , b_w);
         if(b_w == 'b'){
            int b = memory[address];
            return b;
        }
         if (b_w == 'w')
         {
            int t = read_word(memory,address);
            return t;
         }

}