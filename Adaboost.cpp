#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
//import all necessary libraries for this program
using namespace std;

//an array that will be used for storing all of the feature values for each image 
int alldata[16000][4000][2];

//two arrays used for storing the maximum and minimum values for all the features of each image
int minimum[16000];
int maximum[16000];
    
//the main method for this program
int main(int argc, char** argv) {
    //an output stream that will be used for storing the individual classifiers used in the strong classifier
    ofstream myfile3;
    string s= "C:/Users/William/Documents/NetBeansProjects/AdaboostImageLearner/output.txt";
    myfile3.open (s.c_str());
    string line;
    
    int referance=0;
    //separate all the features calculated into 10 chunks as one single array isn't large enough to store all the data necessary
    for(int chunk=0; chunk<10; chunk++){
        //iterate through every face being loaded
        for(int face=0; face<4000; face++){
            //open the current face and store its features in the alldata array
            std::stringstream out;
            out << face;
            string s= "C:/Users/William/Documents/NetBeansProjects/AdaboostImageLearner/Features/Faces/"+out.str()+".txt";
            bool Terminate=false;
            std::ifstream myfile (s.c_str());
            if (myfile.is_open()){
                while ( getline (myfile,line) and Terminate==false ){
                    alldata[referance%16000][face][0]=atoi (line.c_str());;
                    if(alldata[referance%16000][face][0]>maximum[referance%16000]){
                        maximum[referance%16000]=alldata[referance%16000][face][0];
                    }
                    if(alldata[referance%16000][face][0]<minimum[referance%16000]){
                        minimum[referance%16000]=alldata[referance%16000][face][0];
                    }
                    referance++;
                    if(referance>(16000*(chunk+1))){
                        Terminate=true;
                    }
                }
                Terminate=false;
                myfile.close(); //close the file being read
            }else{
                cout << "Unable to open file"; //if there are any errors inform the user
            }
            referance=0;
            //open the current not face features list and store it in the alldata array
            s= "C:/Users/William/Documents/NetBeansProjects/AdaboostImageLearner/Features/Not/"+out.str()+".txt";
            std::ifstream myfile2 (s.c_str());
            if (myfile2.is_open()){
                while ( getline (myfile2,line) and Terminate==false ){
                    alldata[referance%16000][face][1]=atoi (line.c_str());
                    if(alldata[referance%16000][face][1]>maximum[referance%16000]){
                        maximum[referance%16000]=alldata[referance%16000][face][1];
                    }
                    if(alldata[referance%16000][face][1]<minimum[referance%16000]){
                        minimum[referance%16000]=alldata[referance%16000][face][1];
                    }
                    referance++;
                    if(referance>(16000*(chunk+1))){
                        Terminate=true;
                    }
                }
                myfile2.close();//close the file being read
            }else{
                cout << "Unable to open file"; //if an error occurs when reading the data 
            }
            referance=0;
        }
        //inform the user that all the data has bee read into the array
        cout<<"all data has been loaded into the array";
        //iterate through all the features of the images loaded into the array
        for(int feature=0; feature<16000; feature++){
            //create a list of variables used for assessing the strength of the weak classifier currently being assessed
            int outmax=0;
            int outmin=0;
            int lasterr=80000;
            int lastcorrect=-10;
            //iterate through all the possible minimum bounds to find which is the optimum value
            for(int minbound=minimum[feature]; minbound<maximum[feature]; minbound++){
                int correct=0;
                int error_rate=0;
                //check how this minimum boundaries classifier strength works for all images in the array
                for (int loop=0; loop<4000; loop++){
                    //if a false negative occurs increase the error rate
                    if(alldata[feature][loop][0]<minbound){
                        error_rate++;
                    //if a false negative doesn't occur increase correct rate
                    }else{
                        correct++;
                    }
            
                    //if a false positive occurs increase the error rate
                    if(alldata[feature][loop][1]>minbound){
                        error_rate++;
                    //if a false positive doesn't occur increase correct rate
                    }else{
                        correct++;
                    }
                }
                //if the minium boundary has the least amount of errors so far set it to the output strongest boundary
                if(correct>lastcorrect){
                    lastcorrect=correct;
                    lasterr=error_rate;
                    outmin=minbound;
                }
            }
            //iterate through all the possible maximum bounds to find which is the optimum value
            for(int maxbound=maximum[feature]; maxbound>outmin; maxbound--){
                int correct=0;
                int error_rate=0;
                //check how this maximum boundaries classifier strength works for all images in the array
                for (int loop=0; loop<4000; loop++){
                    //if a false negative occurs increase the error rate
                    if(alldata[feature][loop][0]>maxbound or alldata[feature][loop][0]<outmin){
                            error_rate++;
                    //if a false negative doesn't occur increase correct rate
                    }else{
                            correct++;
                    }
                    //if a false positive occurs increase the error rate
                    if(alldata[feature][loop][1]<maxbound and alldata[feature][loop][1]>outmin){
                            error_rate++;
                    //if a false positive doesn't occur increase correct rate
                    }else{
                            correct++;
                    }
                }
                //if the maximum boundary has the least amount of errors so far set it to the output strongest boundary
                if(correct>lastcorrect){
                    lastcorrect=correct;
                    lasterr=error_rate;
                    outmax=maxbound;
                }
            }
            //write the data obtained to an output file
            myfile3 <<outmin<<", "<<outmax<<", "<<lastcorrect<<", "<<lasterr<<", "<<feature+chunk*16000<<" \r\n";
        }
    }
    //close the output file
    myfile3.close();
    return 0;
}

