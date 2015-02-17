#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
//import all the neccesary libraries for this program

using namespace std;
//an array that will be used for storing each integral image
int integralimage[576];

//a method that calculates the area under the region of an image using the integral image
int sumofpixels(int locationx, int locationy, int width, int height){
    return (integralimage[locationx+width+(locationy+height)*24]-integralimage[locationx+(locationy+height)*24]-integralimage[locationx+width+locationy*24]+integralimage[locationx+locationy*24]);
}

//the main method of the program
int main(int argc, char** argv) {

    //manually specific an array storing the details of each Haar feature being used
    int dimensions[5][2];
    dimensions[0][0]=2;
    dimensions[0][1]=1;
    
    dimensions[1][0]=1;
    dimensions[1][1]=2;
    
    dimensions[2][0]=3;
    dimensions[2][1]=1;
    
    dimensions[3][0]=1;
    dimensions[3][1]=3;
    
    dimensions[4][0]=2;
    dimensions[4][1]=2;
    //valiables needed for loading integral images into array
    int count=0;
    string line;
    int face=0;
    //iterate through all the integral images created from the previous programming stage
    for(int face=0; face<6000; face++){
        int referance=0;
        std::stringstream out; //create an output stream for storing data
        out << face;
        string s= "C:/Users/William/Documents/NetBeansProjects/IntegralImage/Integrals/Not/"+out.str()+".txt";
        std::ifstream myfile (s.c_str()); //load the not faces data into the array
        //open the file and read data into an array
        if (myfile.is_open()){
            while ( getline (myfile,line) ){
                if(line==" "){
                    cout<<"error2";
                }            
                integralimage[referance]=atoi (line.c_str());
                referance++;
            }
            myfile.close();//close the input file
        }
        else cout << "Unable to open file"; //display any errors that occur

        //create and load the output file, so it can be written to
        ofstream myfile2;
        s= "C:/Users/William/Documents/NetBeansProjects/AdaboostImageLearner/Features/Not/"+out.str()+".txt";
        myfile2.open (s.c_str());
  
        //iterate through all of the Haar features used in this project
        for(int feat=0; feat<5; feat++){
            //iterate through all of the possible combinations of haar features  scale and position
            for(int x=0; x<(24-(dimensions[feat][0])); x++){
                for(int y=0; y<(24-(dimensions[feat][1])); y++){
                    for(int scale_X=1; scale_X<(24-x)/dimensions[feat][0]; scale_X++){
                        for(int scale_Y=1; scale_Y<(24-y)/dimensions[feat][1]; scale_Y++){
                            //find the value of the current feature being processed
                            count++;
                            int value=0;
                            if(feat==0){
                                value=sumofpixels(x+scale_X,y, scale_X, scale_Y)-sumofpixels(x,y, scale_X, scale_Y);
                            }else if(feat==1){
                                value=sumofpixels(x,y+scale_Y, scale_X, scale_Y)-sumofpixels(x,y, scale_X, scale_Y);
                            }else if(feat==2){
                                value=sumofpixels(x+scale_X,y, scale_X, scale_Y)-sumofpixels(x,y, scale_X, scale_Y)-sumofpixels(x+scale_X*2,y, scale_X, scale_Y);
                            }else if(feat==3){
                                value=sumofpixels(x,y+scale_Y, scale_X, scale_Y)-sumofpixels(x,y, scale_X, scale_Y)-sumofpixels(x,y+scale_Y*2, scale_X, scale_Y);
                            }else if(feat==4){
                                value=sumofpixels(x,y, scale_X, scale_Y)+sumofpixels(x+scale_X,y+scale_Y, scale_X, scale_Y)-sumofpixels(x+scale_X,y, scale_X, scale_Y)-sumofpixels(x,y+scale_Y, scale_X, scale_Y);
                            }
                            //output the value of this feature into the output text file
                            myfile2 <<value<<" \r\n";
                            //checking for any invalid positions being accessed used for debugging
                            int maxx=x+dimensions[feat][0]*scale_X;
                            int maxy=y+dimensions[feat][1]*scale_Y;
                    
                            if(maxx>23 or maxy>23){
                                cout<<"Error";
                            }
                        }
                    }
                }
            }
        }
        //close the output file
        myfile2.close(); 
        //display how many iterations the program completed
        cout<<count<<"\n";
    
        return 0;
}


