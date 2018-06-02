#pragma once
#include <set>
#include "thulac_character.h"
namespace thulac{
    
    class Raw:public std::vector<Character>{
        public:
        Raw& operator+=(Raw& right){
            for(size_t i=0;i<right.size();i++){
                this->push_back(right[i]);
            };
            return *this;
        };
        Raw& operator+=(const char* right){
            while(*right){
                this->push_back(*(right++));
            }
            return *this;
        };
        
        Raw& operator+=(const char& right){
            this->push_back(right);
            return *this;
        };
        Raw& operator+=(const std::string& right){
            for(size_t i=0;i<right.size();i++){
                this->push_back(right[i]);
            };
            return *this;
        };
        
        /*
         * Find the corresponding word from the right, return the subscript, -1 means no
         * */
        int rfind(const Character& sep){
            int ind=this->size()-1;
            while(ind>=0){
                if((*this)[ind]==sep)return ind;
                ind--;
            }
            return -1;
        };
        
        friend std::ostream& operator<< (std::ostream& os,const Raw& raw){
            for(size_t i=0;i<raw.size();i++){
                put_character(raw[i],os);
            }
            return os;
        };
    };
    
    inline void put_raw(const Raw& r,FILE * pFile=stdout){
        for(size_t j=0;j<r.size();j++){
            put_character(r[j],pFile);
        }
    }
    
    inline int string_to_raw(const std::string& str,Raw& sent){
        sent.clear();
        int current_character=-1;
        int c;
        for(int i=0;i<str.length();i++){
            c=str.at(i);
            if(!(c&0x80)){//
                if(current_character!=-1)sent.push_back(current_character);
                current_character=c;//
            }else if(!(c&0x40)){//
                current_character=(current_character<<6)+(c&0x3f);
            }else if(!(c&0x20)){//
                if(current_character!=-1)sent.push_back(current_character);
                current_character=(c&0x1f);
            }else if(!(c&0x10)){//
                if(current_character!=-1)sent.push_back(current_character);
                current_character=(c&0x0f);
            }else if(!(c&0x80)){//
                if(current_character!=-1)sent.push_back(current_character);
                current_character=(c&0x07);
            }else{//
                if(current_character!=-1)sent.push_back(current_character);
                current_character=0;
            }
        }
        if(current_character>0)sent.push_back(current_character);
        return 0;
    }
    
    inline int get_raw(Raw& sent,FILE* pFile=stdin,int min_char=33){
        sent.clear();
        int current_character=-1;
        int c;
        while(1){//
            c=fgetc(pFile);
            if(c==EOF){
                if(current_character!=-1)sent.push_back(current_character);
                return c;//
            }
            if(!(c&0x80)){//
                if(current_character!=-1)sent.push_back(current_character);
                if(c<min_char){//
                    return c;
                }else{//
                    current_character=c;//
                }
            }else if(!(c&0x40)){//
                current_character=(current_character<<6)+(c&0x3f);
            }else if(!(c&0x20)){//
                if(current_character!=-1)sent.push_back(current_character);
                current_character=(c&0x1f);
            }else if(!(c&0x10)){//
                if(current_character!=-1)sent.push_back(current_character);
                current_character=(c&0x0f);
            }else if(!(c&0x80)){//
                if(current_character!=-1)sent.push_back(current_character);
                current_character=(c&0x07);
            }else{//
                if(current_character!=-1)sent.push_back(current_character);
                current_character=0;
            }
        }
    }
    inline int get_raw(Character* seq,int max_len,int&len,FILE* pFile=stdin,int min_char=33){
        len=0;
        Character current_character=-1;
        int c;
        while(1){//
            
            c=fgetc(pFile);
            if(c==EOF){
                if((current_character!=-1)&&(len<max_len))seq[len++]=current_character;
                return c;//
            }
            if(!(c&0x80)){//
                if((current_character!=-1)&&(len<max_len))seq[len++]=current_character;
                if(c<min_char){//
                    return c;
                }else{//
                    current_character=c;//
                }
            }else if(!(c&0x40)){//
                current_character=(current_character<<6)+(c&0x3f);
            }else if(!(c&0x20)){//
                if((current_character!=-1)&&(len<max_len))seq[len++]=current_character;
                current_character=(c&0x1f);
            }else if(!(c&0x10)){//
                if((current_character!=-1)&&(len<max_len))seq[len++]=current_character;
                current_character=(c&0x0f);
            }else if(!(c&0x80)){//
                if((current_character!=-1)&&(len<max_len))seq[len++]=current_character;
                current_character=(c&0x07);
            }else{//
                if((current_character!=-1)&&(len<max_len))seq[len++]=current_character;
                current_character=0;
            }
        }
    }
    
    inline void get_raw(Raw& sent,char* s,int len,int min_char=33){
        sent.clear();
        int current_character=-1;
        int c;
        for(int i = 0; i < len; i ++){//
            c=s[i];
            if(!(c&0x80)){//
                if(current_character!=-1)sent.push_back(current_character);
                if(c<min_char){//
                    //
                    current_character=32;
                }else{//
                    current_character=c;//
                }
            }else if(!(c&0x40)){//
                current_character=(current_character<<6)+(c&0x3f);
            }else if(!(c&0x20)){//
                if(current_character!=-1)sent.push_back(current_character);
                current_character=(c&0x1f);
            }else if(!(c&0x10)){//
                if(current_character!=-1)sent.push_back(current_character);
                current_character=(c&0x0f);
            }else if(!(c&0x80)){//
                if(current_character!=-1)sent.push_back(current_character);
                current_character=(c&0x07);
            }else{//
                if(current_character!=-1)sent.push_back(current_character);
                current_character=0;
            }
        }
        if(current_character>0 && len != 9999){
            sent.push_back(current_character);
        }
        //
        //
    }
    
    
    inline int get_raw(Raw& sent,const std::string& s,int len, int start, int min_char=33){
        sent.clear();
        int current_character=-1;
        int c;
        for(int i = start; i < len; i ++){//
            c=s[i];
            if(!(c&0x80)){//
                if(current_character!=-1)sent.push_back(current_character);
                if(c<min_char){//
                    return i;
                    //
                }else{//
                    current_character=c;//
                }
            }else if(!(c&0x40)){//
                current_character=(current_character<<6)+(c&0x3f);
            }else if(!(c&0x20)){//
                if(current_character!=-1)sent.push_back(current_character);
                current_character=(c&0x1f);
            }else if(!(c&0x10)){//
                if(current_character!=-1)sent.push_back(current_character);
                current_character=(c&0x0f);
            }else if(!(c&0x80)){//
                if(current_character!=-1)sent.push_back(current_character);
                current_character=(c&0x07);
            }else{//
                if(current_character!=-1)sent.push_back(current_character);
                current_character=0;
            }
        }
        if(current_character>0){
            sent.push_back(current_character);
        }
        return -1;
        //
        //
    }
    
    inline int get_raw_vector(std::vector<Raw>& vec,FILE* pFile=stdin,int min_char=33){
        vec.clear();
        Raw sent;
        sent.clear();
        //
        std::set<int> pun_set;
        std::set<int>::iterator it;
        //
        int punInts[] = {63,33,59,12290,65311,65281,65307};
        for(int i = 0; i < 7; i ++){
            pun_set.insert(punInts[i]);
        }
        int current_character=-1;
        int c;
        while(1){//
            c=fgetc(pFile);
            //
            if(c==EOF){
                if(current_character!=-1)sent.push_back(current_character);
                if(sent.size()) vec.push_back(sent);
                return c;//
            }
            if(!(c&0x80)){//
                if(current_character!=-1){
                    sent.push_back(current_character);
                    it = pun_set.find(current_character);
                    if(it != pun_set.end()){
                        //
                        //
                        if(sent.size() == 1){
                            vec.push_back(sent);
                            sent.clear();
                        }else{
                            int last_character = sent[sent.size()-2];
                            it = pun_set.find(last_character);
                            if(it == pun_set.end()){
                                vec.push_back(sent);
                                sent.clear();
                            }
                        }
                    }
                }
                if(c<min_char){//
                    if(sent.size()) vec.push_back(sent);
                    //
                    return c;
                    //
                }else{//
                    current_character=c;//
                }
            }else if(!(c&0x40)){//
                current_character=(current_character<<6)+(c&0x3f);
            }else if(!(c&0x20)){//
                //
                if(current_character!=-1){
                    sent.push_back(current_character);
                    it = pun_set.find(current_character);
                    if(it != pun_set.end()){
                        //
                        //
                        if(sent.size() == 1){
                            vec.push_back(sent);
                            sent.clear();
                        }else{
                            int last_character = sent[sent.size()-2];
                            it = pun_set.find(last_character);
                            if(it == pun_set.end()){
                                vec.push_back(sent);
                                sent.clear();
                            }
                        }
                    }
                }
                current_character=(c&0x1f);
            }else if(!(c&0x10)){//
                //
                if(current_character!=-1){
                    sent.push_back(current_character);
                    it = pun_set.find(current_character);
                    if(it != pun_set.end()){
                        //
                        //
                        if(sent.size() == 1){
                            vec.push_back(sent);
                            sent.clear();
                        }else{
                            int last_character = sent[sent.size()-2];
                            it = pun_set.find(last_character);
                            if(it == pun_set.end()){
                                vec.push_back(sent);
                                sent.clear();
                            }
                        }
                    }
                }
                current_character=(c&0x0f);
            }else if(!(c&0x80)){//
                //
                if(current_character!=-1){
                    sent.push_back(current_character);
                    it = pun_set.find(current_character);
                    if(it != pun_set.end()){
                        //
                        //
                        if(sent.size() == 1){
                            vec.push_back(sent);
                            sent.clear();
                        }else{
                            int last_character = sent[sent.size()-2];
                            it = pun_set.find(last_character);
                            if(it == pun_set.end()){
                                vec.push_back(sent);
                                sent.clear();
                            }
                        }
                    }
                }
                current_character=(c&0x07);
            }else{//
                //
                if(current_character!=-1){
                    sent.push_back(current_character);
                    it = pun_set.find(current_character);
                    if(it != pun_set.end()){
                        //
                        if(sent.size() == 1){
                            vec.push_back(sent);
                            sent.clear();
                        }else{
                            int last_character = sent[sent.size()-2];
                            it = pun_set.find(last_character);
                            if(it == pun_set.end()){
                                vec.push_back(sent);
                                sent.clear();
                            }
                        }
                    }
                }
                current_character=0;
            }
        }
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
    }
    
    
    inline void cut_raw(Raw& sent, std::vector<Raw>& vec, int max_len){
        vec.clear();
        //
        Raw sent_tmp;
        std::set<int> pun_set;
        std::set<int>::iterator it;
        //
        int punInts[] = {63,33,59,12290,65311,65281,65307};
        for(int i = 0; i < 7; i ++){
            pun_set.insert(punInts[i]);
        }
        int current_character=-1;
        int c, num = 0, last_pun = 0;
        sent_tmp.clear();
        for(int i = 0; i < sent.size(); i++){//
            c = sent[i];
            num++;
            it = pun_set.find(c);
            if(it != pun_set.end() || i == sent.size()-1) {
                if(num > max_len) {
                    vec.push_back(sent_tmp);
                    sent_tmp.clear();
                    num = i - last_pun + 1;
                }
                for(int j = last_pun; j <= i; j++) sent_tmp.push_back(sent[j]);
                last_pun = i+1;
            }
        }
        if(sent_tmp.size()) vec.push_back(sent_tmp);
    }
    
}//
