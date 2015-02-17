/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package integralimage;

import java.awt.FlowLayout;
import java.awt.image.BufferedImage;
import java.awt.image.WritableRaster;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import javax.imageio.ImageIO;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;

public class IntegralImage {
    
    static int[] integral(BufferedImage i, int number) throws IOException{
        //the array being outputted to the file (the integral image)
        int out[]=new int[24*24];
        //populate the origin point of the image
        out[0]=i.getData().getSample(0, 0, 0);
        //create an ouput stream for the image being processed
        File output = new File("C:/Users/William/Documents/NetBeansProjects/IntegralImage/Integrals/Not/"+Integer.toString(number)+".txt");
 	//if the file tring to be written to doesnt exist
        if (!output.exists()) {
            output.createNewFile();
	}
        //create an output buffer
        FileWriter fw = new FileWriter(output.getAbsoluteFile());
	BufferedWriter bw = new BufferedWriter(fw);
	bw.write(Integer.toString(out[0]));
        bw.newLine();
        //write the integral image values for each axis of the image   
        for(int x=1; x<24; x++){
            out[x]=out[x-1]+i.getData().getSample(x, 0, 0);
                 bw.write(Integer.toString(out[x]));
                 bw.newLine();
           
        }
        for(int y=0; y<24; y++){
            out[y*24]=out[(y)*24]+i.getData().getSample(0, y, 0);
        }
        //iterate through all the points of the image
        for(int y=1; y<24; y++){
            bw.write(Integer.toString(out[y*24]));
            bw.newLine();
            //write the output for the integral image of all points in the input image
            for(int x=1; x<24; x++){
                for(int lx=0; lx<x+1; lx++){
                    for(int ly=0; ly<y+1; ly++){
                        out[y*24+x]+=i.getData().getSample(lx, ly, 0);
                    }
                }
                bw.write(Integer.toString(out[y*24+x]));
                bw.newLine();
              
            }
        }
        //close the output file
        bw.close();
        return out;
    }
    //the main method
    public static void main(String[] args) {
        //an object that will store the raw image data
        BufferedImage img;
        for(int number=0; number<6000; number++){
            try {
                //load the current image being processed
                img = ImageIO.read(new File("C:/Users/William/Documents/NetBeansProjects/IntegralImage/Finished Data/Not faces/"+Integer.toString(number)+".bmp"));
                //inform the user of the progress of the program
                System.out.println(number);
                //calculate and output the integral image of the image
                integral(img,number);
            //if the file cat be found output a error message
            }catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
