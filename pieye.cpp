#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include "camera.h"
#include "graphics.h"

using namespace std;


#define MAIN_TEXTURE_WIDTH 64
#define MAIN_TEXTURE_HEIGHT 64

char tmpbuff[MAIN_TEXTURE_WIDTH*MAIN_TEXTURE_HEIGHT*4];
char outbuff[MAIN_TEXTURE_WIDTH*MAIN_TEXTURE_HEIGHT*4];
char greyscale[MAIN_TEXTURE_WIDTH*MAIN_TEXTURE_HEIGHT];
int integral[MAIN_TEXTURE_WIDTH*MAIN_TEXTURE_HEIGHT];
int size=64;

int features[100][7];
int dimensions[5][2];




int sum(int x, int y){
	return (-integral[(x-1)+(y-1)*size]+integral[(x-1)+y*size]+integral[x+(y-1)*size]+greyscale[x+y*size]);
}

int area(int x, int y, int width, int height){
	return(integral[x+width+(y+height)*size]-integral[x+(y+height)*size]-integral[x+width+y*size]+integral[x+y*size]);
}

int feature(int type, int x, int y, int scalex, int scaley){
	int result=0;
	
	if(type==0){
		result=area(x+scalex, y, scalex, scaley)-area(x, y, scalex, scaley);
	}else if(type==1){
		result=area(x, y+scaley, scalex, scaley)-area(x, y, scalex, scaley);
	}else if(type==2){
		result=area(x+scalex, y, scalex, scaley)-area(x, y, scalex, scaley)-area(x+scalex*2, y, scalex, scaley);
	}else if(type==3){
		result=area(x, y+scaley, scalex, scaley)-area(x, y, scalex, scaley)-area(x, y+scaley*2, scalex, scaley);
	}else if(type==4){
		result=area(x, y, scalex, scaley)+area(x+scalex, y+scaley, scalex, scaley)-area(x+scalex, y, scalex, scaley)-area(x, y+scaley, scalex, scaley);
	}
	return result;
}

void find_integral(){
	integral[0]=greyscale[0];
	for(int loop=0; loop<size-1; loop++){
		integral[loop+1]=integral[loop]+greyscale[loop+1];
		integral[(loop+1)*size]=integral[loop*size]+greyscale[size*(loop+1)];

	}
	for(int y=1; y<size; y++){
		for(int x=1; x<size; x++){
			integral[x+y*size]=sum(x, y);

		}
	}
}


void loadinput(string filename){
	
	std::ifstream datafile (filename.c_str());
    	string line;
	string accumulator;
    	int criteria=0;
	int linecount;
    	if (datafile.is_open()){
        	while ( getline (datafile,line)){
            		for(int loop=0; loop<line.length(); loop++){
				if(line[loop]!=9){
					accumulator+=line[loop];
				}else if(accumulator!=""){
					printf("data %i", criteria);
					features[linecount][criteria]=atoi(accumulator.c_str());
					accumulator="";
					criteria++;
				}
			}
			features[linecount][criteria]=atoi(accumulator.c_str());
			linecount++;
			criteria=0;

        	}
   	}
	datafile.close();
}



int main(int argc, const char** argv){
	loadinput("/home/pi/data.txt");
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

	features[0][0]=-6;
	features[0][1]=9;
	features[0][2]=1;
		
	
	//init graphics and the camera
	InitGraphics();
	printf("Data");
	CCamera* cam = StartCamera(MAIN_TEXTURE_WIDTH, MAIN_TEXTURE_HEIGHT,30,1,true);
	
	//create 4 textures of decreasing size
	GfxTexture textures[1];
	textures[0].Create(MAIN_TEXTURE_WIDTH, MAIN_TEXTURE_HEIGHT);

	int left=0;
	int right=0;
	int upleft=0;
	int upright=0;
	printf("Running frame loop\n");
	for(int i = 0; i < 3000; i++)
	{
		//lock the chosen frame buffer, and copy it directly into the corresponding open gl texture
		void const* frame_data; int frame_sz;
		if(cam->BeginReadFrame(0,frame_data, frame_sz))
		{




				memcpy(tmpbuff, frame_data, frame_sz);
				//char outbuff[MAIN_TEXTURE_WIDTH*MAIN_TEXTURE_HEIGHT*4];

				for (int loop=0; loop<MAIN_TEXTURE_WIDTH*MAIN_TEXTURE_HEIGHT*4; loop+=4){
				int red=tmpbuff[loop];
				int green=tmpbuff[loop+1];
				int blue=tmpbuff[loop+2];


				int average=(red+green+blue)/3;
				tmpbuff[loop]=average;
				tmpbuff[loop+1]=average;
				tmpbuff[loop+2]=average;
				greyscale[loop/4]=average;
				}			


			find_integral();
			int potential_eyes[250][2];
			for(int xoff=0; xoff<size-24; xoff++){
				for(int yoff=0; yoff<size-24; yoff++){
					int correct=0;
					for(int loop=0; loop<100; loop++){
						int value=0;
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
						if((value<features[loop][1] or features[loop][1]==0) and value>features[loop][0]){
							correct++;
						}
					}

//					printf("number %i \n", correct);

					if(correct>=38){
						int darkest=255;
						int posx=0;
						int posy=0;
//						printf("number %i", correct);
						
						int eyeref=0;
						for(int yy=yoff; yy<(yoff+24); yy++){
							for(int xx=xoff; xx<(xoff+24); xx++){
							//	if((greyscale[xx+yy*size])<darkest){
									tmpbuff[(xx+yy*size)*4]=0;
								//	outbuff[(xx+yy*size)*4+1]=tmpbuff[(xx+yy*size)*4+1];
								//	outbuff[(xx+yy*size)*4+2]=tmpbuff[(xx+yy*size)*4+2];
								//	outbuff[(xx+yy*size)*4+3]=tmpbuff[(xx+yy*size)*4+3];



								if(greyscale[xx+yy*size]<25 and greyscale[xx+yy*size]>15){
									tmpbuff[(xx+yy*size)*4+2]=0;
									tmpbuff[(xx+yy*size)*4+1]=255;
									tmpbuff[(xx+yy*size)*4]=0;
									potential_eyes[eyeref][0]=xx;
									potential_eyes[eyeref][1]=yy;
									eyeref++;
								}

								//}
							}
						}
						int newleft=0;
						int newright=0;
						int newupleft=0;
						int newupright=0;
						int totalpossible=0;
						for(int eyeloop=0; eyeloop<25; eyeloop++){
							if(((potential_eyes[eyeloop+1][0]-potential_eyes[eyeloop][0])>3 and (potential_eyes[eyeloop+1][0]-potential_eyes[eyeloop][0])<10) and potential_eyes[eyeloop+1][1]==potential_eyes[eyeloop][1]){
								tmpbuff[((potential_eyes[eyeloop][0])+potential_eyes[eyeloop][1]*size)*4+2]=0;							
								tmpbuff[((potential_eyes[eyeloop][0])+potential_eyes[eyeloop][1]*size)*4+1]=0;
								tmpbuff[((potential_eyes[eyeloop][0])+potential_eyes[eyeloop][1]*size)*4]=255;
								tmpbuff[((potential_eyes[eyeloop+1][0])+potential_eyes[eyeloop+1][1]*size)*4+2]=0;
								tmpbuff[((potential_eyes[eyeloop+1][0])+potential_eyes[eyeloop+1][1]*size)*4+1]=0;
								tmpbuff[((potential_eyes[eyeloop+1][0])+potential_eyes[eyeloop+1][1]*size)*4]=255;
								totalpossible++;
								newleft+=potential_eyes[eyeloop][0];
								newright+=potential_eyes[eyeloop+1][0];
								newupleft+=potential_eyes[eyeloop+1][1];
								newupright+=potential_eyes[eyeloop+1][1];
							}
						}
						if(totalpossible>1){
							newleft/=totalpossible;
							newright/=totalpossible;
							newupleft/=totalpossible;
							newupright/=totalpossible;
						}
						if(newright-right>1 and newleft-left>1){
							printf("right \n");
						}else if(right-newright>1 and left-newleft>1){
							printf("left \n");
						}
						if(newupright-upright>1 and newupleft-upleft>1){
							printf("up \n");
						}else if(upright-newupright>1 and upleft-newupleft>1){
							printf("down \n");
						}
//						printf("right %i %i \n", newright, right);
						if(i%10==0){
							right=newright;
							left=newleft;
							upright=newupright;
							upleft=newupleft;
						}
					//	outbuff[(posx+posy*size)*4]=0;
					//	tmpbuff[(xoff+yoff*size)*4]=0;
					//	tmpbuff[(xoff+24+yoff*size)*4]=0;
					//	tmpbuff[(xoff+(yoff+24)*size)*4]=0;
					//	tmpbuff[(xoff+24+(yoff+24)*size)*4]=0;
					}
				}
			}
			textures[0].SetPixels(tmpbuff);
			cam->EndReadFrame(0);
		}

		//begin frame, draw the texture then end frame (the bit of maths just fits the image to the screen while maintaining aspect ratio)
		BeginFrame();
		float aspect_ratio = float(MAIN_TEXTURE_WIDTH)/float(MAIN_TEXTURE_HEIGHT);
		float screen_aspect_ratio = 1280.f/720.f;
		DrawTextureRect(&textures[0],-aspect_ratio/screen_aspect_ratio,-1.f,aspect_ratio/screen_aspect_ratio,1.f);
		EndFrame();
	}

	StopCamera();
}



