#include <boost/filesystem.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <tuple>
#include <fstream>
using namespace std;


void saveTestCases(int id, vector<string> &inputs, vector<string> &outputs){
    string folderPath = "exercises/" + to_string(id);
    bool res = boost::filesystem::create_directories(folderPath);
    if(res)
        cout << "exercise folder successfully created " << endl;
    for (int i = 0; i < inputs.size(); i++){
        string filename = folderPath + "/" + to_string(i);
        ofstream f_out(filename + ".out");
        f_out << outputs[i];
        ofstream f_in(filename + ".in");
        f_in << inputs[i];
        f_out.close();
        f_in.close();
    }

}

void removeTestCases(int id){
    string folderPath = "exercises/" + to_string(id);
    if(boost::filesystem::remove_all(folderPath))
        cout << "Test cases removed " << endl;
}

void createCompilationFolder(){
    boost::filesystem::create_directory("compile_dir");
}

void saveSubmit(int ex_id, int usr_id, string submit){
    string folderPath = "exercises/" + to_string(ex_id) + "/submits";
    boost::filesystem::create_directory(folderPath);
    ofstream f{folderPath + "/" + to_string(usr_id) + ".prog"};
    f << submit;
}