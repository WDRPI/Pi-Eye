#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include "camera.h"
#include "graphics.h"
//import all the neccesary libraries for this project

using namespace std;


#define MAIN_TEXTURE_WIDTH 64
#define MAIN_TEXTURE_HEIGHT 64
//set the dimensions of the resolution of input frame from the camera


char tmpbuff[MAIN_TEXTURE_WIDTH*MAIN_TEXTURE_HEIGHT*4]; //an array that will store the input data
char greyscale[MAIN_TEXTURE_WIDTH*MAIN_TEXTURE_HEIGHT]; //an array that will store the normalised and greyscale version of the input video data
int integral[MAIN_TEXTURE_WIDTH*MAIN_TEXTURE_HEIGHT]; //an array used for storing the integral image of each frame of data
int MAIN_TEXTURE_WIDTH=64;

int features[100][7]; //an array storing all the characteristics of the weak classifiers making up the strong classifier being used
int dimensions[5][2]; //an array storing the dimensions for each individual haar featre being used




int sum(int x, int y){ //a function that will caluclate the area of an individual pixel in the image array
	return (-integral[(x-1)+(y-1)*MAIN_TEXTURE_WIDTH]+integral[(x-1)+y*MAIN_TEXTURE_WIDTH]+integral[x+(y-1)*MAIN_TEXTURE_WIDTH]+greyscale[x+y*MAIN_TEXTURE_WIDTH]);
}

int area(int x, int y, int width, int height){ //a function that will calculate the area of a defined region of the grescale image
	return(integral[x+width+(y+height)*MAIN_TEXTURE_WIDTH]-integral[x+(y+height)*MAIN_TEXTURE_WIDTH]-integral[x+width+y*MAIN_TEXTURE_WIDTH]+integral[x+y*MAIN_TEXTURE_WIDTH]);
}
//a function that will return the value for each individual feature of the image
int feature(int type, int x, int y, int scalex, int scaley){
	int result=0;//the output variable
	
	if(type==0){//if using the 1st tpe of haar feature
		result=area(x+scalex, y, scalex, scaley)-area(x, y, scalex, scaley) //compare 2 horizontally adjacent areas;
	}else if(type==1){//if using the 2nd type of haar feature
		result=area(x, y+scaley, scalex, scaley)-area(x, y, scalex, scaley) //compare 2 vertically adjacent areas;
	}else if(type==2){//if using the 3rd type of haar featue
		result=area(x+scalex, y, scalex, scaley)-area(x, y, scalex, scaley)-area(x+scalex*2, y, scalex, scaley); //compare 3 horizontal adjacent areas
	}else if(type==3){//if using the 4th type of haar feature
		result=area(x, y+scaley, scalex, scaley)-area(x, y, scalex, scaley)-area(x, y+scaley*2, scalex, scaley); //compare 3 vertically adjacent pixels
	}else if(type==4){//if using the 5th type of haar feature
		result=area(x, y, scalex, scaley)+area(x+scalex, y+scaley, scalex, scaley)-area(x+scalex, y, scalex, scaley)-area(x, y+scaley, scalex, scaley); //compare 4 diagonal areas
	}
	return result; //ouptut the result
}

void find_integral(){ //a function which will compute the integral image of the image from the camera feed
	integral[0]=greyscale[0]; //populate the first element of the integral image with the first element of the normal image
	for(int loop=0; loop<MAIN_TEXTURE_WIDTH-1; loop++){ //calculate the X and Y border integrals from the image data
		integral[loop+1]=integral[loop]+greyscale[loop+1];
		integral[(loop+1)*MAIN_TEXTURE_WIDTH]=integral[loop*MAIN_TEXTURE_WIDTH]+greyscale[MAIN_TEXTURE_WIDTH*(loop+1)];

	}
        //calculate the integral image for the rest of the X and Y points within the raw data image
	for(int y=1; y<MAIN_TEXTURE_WIDTH; y++){
		for(int x=1; x<MAIN_TEXTURE_WIDTH; x++){
			integral[x+y*MAIN_TEXTURE_WIDTH]=sum(x, y);

		}
	}
}

//a method that will load all of the weak classifiers from an external text file
void loadinput(string filename){
	
	std::ifstream datafile (filename.c_str()); //create an input stream
    	string line;
	string accumulator;
    	int criteria=0;
	int linecount;
    	if (datafile.is_open()){ //open the file
        	while ( getline (datafile,line)){  //read each line
            		for(int loop=0; loop<line.length(); loop++){ //iterate through each character
				if(line[loop]!=9){
					accumulator+=line[loop];
				}else if(accumulator!=""){
					features[linecount][criteria]=atoi(accumulator.c_str()); //store each property of each weak classifier in the array features
					accumulator="";
					criteria++;
				}
			}
			features[linecount][criteria]=atoi(accumulator.c_str());
			linecount++;
			criteria=0;

        	}
   	}
	datafile.close(); //close the input file
}


//the main method
int main(int argc, const char** argv){
	loadinput("/home/pi/data.txt"); //call the method that will load the weak classifiers
	
        //manually populate the properties of each of the 5 haar features being used in this program
        dimensions[0][1]=2;
    	dimensions[0][1]=1;
    
	dimensions[1][0]=1;
    	dimensions[1][1]=2;
    
    	dimensions[2][0]=3;
    	dimensions[2][1]=1;
    
    	dimensions[3][0]=1;
    	dimensions[3][1]=3;
    
    	dimensions[4][0]=2;
    	dimensions[4][1]=2;


        //initalise the GPU/output buffer
	InitGraphics();
        //create an instance of the camera library
	CCamera* cam = StartCamera(MAIN_TEXTURE_WIDTH, MAIN_TEXTURE_HEIGHT,30,1,true);
	//create an initialise an output texture to be displayed on the screen
        GfxTexture textures[1];
	textures[0].Create(MAIN_TEXTURE_WIDTH, MAIN_TEXTURE_HEIGHT);
        //variables that will store the positions in terms of X and Y coordinates of each detected eye
	int left=0;
	int right=0;
	int upleft=0;
	int upright=0;
        //the main loop that will be where most of the commands are executed
	for(int i = 0; i < 3000; i++){
		//initalise objects that will store data from the camera
                void const* frame_data; int frame_sz;
                //if the library was able to retrieve data from the camera feed
		if(cam->BeginReadFrame(0,frame_data, frame_sz)){

                                //copy all the image data into the tmpbuff array so that it can be programmatically manipulated
				memcpy(tmpbuff, frame_data, frame_sz);
                                //iterate through all the items in the image array
				for (int loop=0; loop<MAIN_TEXTURE_WIDTH*MAIN_TEXTURE_HEIGHT*4; loop+=4){
                                    //convert the image to greyscale
                                    int red=tmpbuff[loop];
                                    int green=tmpbuff[loop+1];
                                    int blue=tmpbuff[loop+2];
                                    int average=(red+green+blue)/3;
                                    tmpbuff[loop]=average;
                                    tmpbuff[loop+1]=average;
                                    tmpbuff[loop+2]=average;
                                    //populate the greyscale array with the greyscale values of the image
                                    greyscale[loop/4]=average;
				}			
        			//find the integral array
                                find_integral();
                                //an array that will be used to store the location of potential points of eyes
                		int potential_eyes[250][2];
                                //iterate through all potential 24*24 areas of pixels the captured image
                                for(int xoff=0; xoff<MAIN_TEXTURE_WIDTH-24; xoff++){
                                    for(int yoff=0; yoff<MAIN_TEXTURE_WIDTH-24; yoff++){
                                        //a variable used to track the amount of features successfully validated by each input image
                                        int correct=0;
                                        //iterate through the top 10 weak classifiers forming a string classifier and evaluate each feature for every possible 24*24 area
					for(int loop=0; loop<100; loop++){
						//a value containing the output of the integral image 
                                                int value=0;
                                                //evaluate the weak classifier for the current 24*24 area being processed
						int x=features[loop][3]+xoff;
						int y=features[loop][4]+yoff;
						int scale_X=features[loop][5];
						int scale_Y=features[loop][6];
						if(features[loop][2]==0){
							value=area(x+scale_X,y, scale_X, scale_Y)-area(x,y, scale_X, scale_Y);
						}else if(features[loop][2]==1){
							value=area(x,y+scale_Y, scale_X, scale_Y)-area(x,y, scale_X, scale_Y);
						}else if(features[loop][2]==2){
							value=area(x+scale_X,y, scale_X, scale_Y)-area(x,y, scale_X, scale_Y)-area(x+scale_X*2,y, scale_X, scale_Y);
						}else if(features[loop][2]==3){
							value=area(x,y+scale_Y, scale_X, scale_Y)-area(x,y, scale_X, scale_Y)-area(x,y+scale_Y*2, scale_X, scale_Y);
						}else if(features[loop][2]==4){
							value=area(x,y, scale_X, scale_Y)+area(x+scale_X,y+scale_Y, scale_X, scale_Y)-area(x+scale_X,y, scale_X, scale_Y)-area(x,y+scale_Y, scale_X, scale_Y);
						}
                                                //if the feature applies to the current 24*24 area increment the amount of correct features for this area
						if((value<features[loop][1] or features[loop][1]==0) and value>features[loop][0]){
							correct++;
						}
					}

                                        //if the amount of correct classifiers for the 24*24 area is above the give a found optimum threshold
					if(correct>=38){
                                                // a variale tracking potential eyes
						int eyeref=0;
                                                //iterate through the 24*24 grid selected
						for(int yy=yoff; yy<(yoff+24); yy++){
							for(int xx=xoff; xx<(xoff+24); xx++){
                                                                //apply a blue filter to the region where a face exists
                                                                tmpbuff[(xx+yy*MAIN_TEXTURE_WIDTH)*4]=0;
                                                                //if it is a eye then apply a seperate filter to both eyes
								if(greyscale[xx+yy*MAIN_TEXTURE_WIDTH]<25 and greyscale[xx+yy*MAIN_TEXTURE_WIDTH]>15){
									tmpbuff[(xx+yy*MAIN_TEXTURE_WIDTH)*4+2]=0;
									tmpbuff[(xx+yy*MAIN_TEXTURE_WIDTH)*4+1]=255;
									tmpbuff[(xx+yy*MAIN_TEXTURE_WIDTH)*4]=0;
									potential_eyes[eyeref][0]=xx;
									potential_eyes[eyeref][1]=yy;
									eyeref++;
								}
							}
						}
                                                //variables storing the current position of the eyes
						int newleft=0;
						int newright=0;
						int newupleft=0;
						int newupright=0;
						int totalpossible=0;
                                                //iterate through all potential eyes
						for(int eyeloop=0; eyeloop<25; eyeloop++){
                                                        //if its not a fixed block, apears to be an eye apply a seperate filter
							if(((potential_eyes[eyeloop+1][0]-potential_eyes[eyeloop][0])>3 and (potential_eyes[eyeloop+1][0]-potential_eyes[eyeloop][0])<10) and potential_eyes[eyeloop+1][1]==potential_eyes[eyeloop][1]){
								tmpbuff[((potential_eyes[eyeloop][0])+potential_eyes[eyeloop][1]*MAIN_TEXTURE_WIDTH)*4+2]=0;							
								tmpbuff[((potential_eyes[eyeloop][0])+potential_eyes[eyeloop][1]*MAIN_TEXTURE_WIDTH)*4+1]=0;
								tmpbuff[((potential_eyes[eyeloop][0])+potential_eyes[eyeloop][1]*MAIN_TEXTURE_WIDTH)*4]=255;
								tmpbuff[((potential_eyes[eyeloop+1][0])+potential_eyes[eyeloop+1][1]*MAIN_TEXTURE_WIDTH)*4+2]=0;
								tmpbuff[((potential_eyes[eyeloop+1][0])+potential_eyes[eyeloop+1][1]*MAIN_TEXTURE_WIDTH)*4+1]=0;
								tmpbuff[((potential_eyes[eyeloop+1][0])+potential_eyes[eyeloop+1][1]*MAIN_TEXTURE_WIDTH)*4]=255;
								//increment the amount of possible pairs of eyes
                                                                totalpossible++;
								newleft+=potential_eyes[eyeloop][0];
								newright+=potential_eyes[eyeloop+1][0];
								newupleft+=potential_eyes[eyeloop+1][1];
								newupright+=potential_eyes[eyeloop+1][1];
							}
						}
                                                //avoid divide by 0 error
						if(totalpossible>1){
                                                        //average the eye positions to get a better approximation
							newleft/=totalpossible;
							newright/=totalpossible;
							newupleft/=totalpossible;
							newupright/=totalpossible;
						}
                                                //if the eye moves to the right output right to the client
						if(newright-right>1 and newleft-left>1){
							printf("right \n");
                                                //if the eye moves to the left output left to the client
						}else if(right-newright>1 and left-newleft>1){
							printf("left \n");
						}
                                                //if the eye moves up, output up to the client
						if(newupright-upright>1 and newupleft-upleft>1){
							printf("up \n");
                                                //if the eye moves down, output down to the client
						}else if(upright-newupright>1 and upleft-newupleft>1){
							printf("down \n");
						}
						//reposition the central position of the eye after 10 iterations
                                                if(i%10==0){
							right=newright;
							left=newleft;
							upright=newupright;
							upleft=newupleft;
						}
					}
				}
			}
                        //output the image data to the screen
			textures[0].SetPixels(tmpbuff);
			cam->EndReadFrame(0);
		}

		//construct a frame
                BeginFrame();
                //calcualte the correct aspect ratio for the screen
		float aspect_ratio = float(MAIN_TEXTURE_WIDTH)/float(MAIN_TEXTURE_HEIGHT);
		float screen_aspect_ratio = 1280.f/720.f;
                //draw the texture to the screen
		DrawTextureRect(&textures[0],-aspect_ratio/screen_aspect_ratio,-1.f,aspect_ratio/screen_aspect_ratio,1.f);
		//finish drawing to the frame
                EndFrame();
	}
        //stop receiving data form the camera
	StopCamera();
}



