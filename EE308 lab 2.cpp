#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstdlib> 
#include <stack>
using namespace::std;
const string keywords[] = {"auto","break","case","char","const","continue","default","do",
"double","else","enum","extern","float","for","goto","if","int",
"long","register","return","short","signed","sizeof","static",
"struct","switch","typedef","union","unsigned","void","volatile","while"};
vector<string> file_content;  //构建关键词容器 

void file_read(string filename){ //进行内容读取 
	ifstream inFile;
	string line;
	inFile.open(filename.c_str());
	if (!inFile.is_open()){
		cout << "\nThe file was not successfully opened"
		<< "\n Please check that the file currently exists."
		<< endl;
		exit(1);
	} // 检查是否成功打开
	while (getline(inFile, line)) {
        file_content.push_back(line);
    }
    inFile.close();     //关闭文件
}

void delete_useless_content(){   //进行无用内容的删除 
	string Line,words;
	int index;
	size_t tabs_location,annotation_location,quot_location,star1_location,star2_location;
	for(int i=0;i<file_content.size(); i++){
		Line=file_content[i];
		if(Line.find("\t")!=Line.npos&&Line.find("\t")==0){
			do{
				tabs_location=Line.find("\t")+1;
				Line=Line.substr(tabs_location,Line.length()-tabs_location);
			}while(Line.find("\t") != Line.npos && Line.find("\t") == 0);
			file_content[i] = Line;
		}
		if (Line.find("//") != Line.npos) {    
            annotation_location=Line.find("//");       
            if (annotation_location==0) {
                file_content.erase(file_content.begin()+i);      
                i--;
            } else {
                words = Line.substr(0,annotation_location);       
                file_content[i] = words;
            }
        }else if(Line.find("\"") !=Line.npos){
        	size_t temp[50]={0};
        	tabs_location=0;
        	index=0;
        	while ((quot_location=Line.find("\"",tabs_location)) != Line.npos) {
        		temp[index]=quot_location;
        		index++;
        		tabs_location=quot_location+1;
			}
			temp[index]=Line.length();
			words= Line.substr(0,temp[0]);
        	for (int j=1; temp[j]!=0; j+=2) {
                words += Line.substr(temp[j]+1,temp[j+1]-temp[j]-1);      
            }
			file_content[i]=words;	
		} else if(Line.find("/*") != Line.npos){
			star1_location=Line.find("/*");
            star2_location= Line.find("*/");
            if (star2_location!= Line.npos) {   
                if (star1_location== 0) {
                    file_content[i] = Line.substr(star2_location+2,Line.length());
                } else {
                    file_content[i] = Line.substr(0,star1_location);
                }
                i--;
            }else{
            	file_content[i] = Line.substr(0,star1_location);
                i += 1;
                Line = file_content[i];
                while (Line.find("*/") == Line.npos) {
                    file_content.erase(file_content.begin()+i);     
                    Line = file_content[i];
                }
                star2_location = Line.find("*/")+2;
                file_content[i] = Line.substr(star2_location,Line.length()-star2_location);
			}
			
		}
	}
}
bool independence_judge(string line,long i){ // 进行关键字的判断 
	if (line[i] < 48 || (line[i] > 57 && line[i] < 65) || (line[i] > 90 && line[i] < 97) ||line[i] >122) {
        return true;
    } else {
        return false;
    }
}
bool involve_judge(string line, string word){
    size_t word_location;
    word_location=line.find(word);
    if (word_location == 0) {
        if (independence_judge(line, word.length()) || word.length() == line.length()) {
            return true;
        } else {
            return false;
        }
    } else {
        if (independence_judge(line, word_location-1) && (independence_judge(line, word_location+word.length()) || word_location+word.length() == line.length())) {
            return true;
        } else {
            return false;
        }
    }
    
}
void cal_keywords_num(){ //计算关键字的数量
	int keywords_num=0;
	string line;
	for (int i = 0; i<file_content.size(); i++) {
        line = file_content[i];
        for (int j=0; j<32; j++) {
            size_t keywords_location = line.find(keywords[j], 0);
            while (keywords_location!=line.npos && involve_judge(line,keywords[j])) {
                keywords_num++;
                keywords_location = line.find(keywords[j],keywords_location + 1);
            }
        }
    }
    cout<<"total num: "<<keywords_num<<endl;
}
void cal_switch_case_num(){ //"switch-case"结构的计算 
    int switch_num = 0,last = -1,case_num[200]={0};
    string line;
    for (int i = 0; i<file_content.size(); i++) {
        line = file_content[i];
        if (line.find("switch") != line.npos && involve_judge(line, "switch")) {        
            switch_num += 1;
            last += 1;
        }
        if (line.find("case") != line.npos && involve_judge(line, "case")) {        /*2é?òcase*/
            case_num[last] += 1;
        }
    }
    cout<<"switch num: "<<switch_num<<endl;
    cout<<"case num:";
    for (int j = 0; j<=last; j++) {
        cout<<" "<<case_num[j];
    }
    cout<<endl;	
}

void cal_if_else_num(int level) { //"if-else"结构的计算 
    int if_else_num = 0,if_elseif_else_num= 0;
    stack<int> s;
    string line;
    for (int i = 0; i<file_content.size(); i++) {
        line = file_content[i];
        if (line.find("if") != line.npos && line.find("else") == line.npos && involve_judge(line, "if")) {      
            s.push(1);
        } else if (line.find("if") == line.npos && line.find("else") != line.npos &&  s.empty() == false) {
            if (s.top() == 1) {
                if_else_num++;    
            } else {
                if_elseif_else_num++;    
            }
            s.pop();       
            
        }else  if (line.find("if") != line.npos && line.find("else") != line.npos && involve_judge(line, "if")) {
            s.push(2);    
        }
    }
    if (level == 3) {
        cout<<"if-else num: "<<if_else_num<<endl;
    } else if (level == 4) {
        cout<<"if-else num: "<<if_else_num<<endl;
        cout<<"if-else-if num: "<<if_elseif_else_num<<endl;
    }
}
int main() {
    int level = 0;
    string filename;
    cout<<"Please enter the path of the code file: "<<endl;
	cin>>filename;
	cout<<"Please enter the level: ";
	cin>>level;

    
    file_read(filename);
    delete_useless_content();
    
    switch (level) {
        case 1:
            cal_keywords_num();
            break;
        case 2:
            cal_keywords_num();
            cal_switch_case_num();
            break;
        case 3:
            cal_keywords_num();
            cal_switch_case_num();
            cal_if_else_num(3);
            break;
        case 4:
            cal_keywords_num();
            cal_switch_case_num();
            cal_if_else_num(4);
            break;
        default:
            cout<<"Please enter a number between 1 and 4"<<endl;
            break;
    }

    return 0;
}
