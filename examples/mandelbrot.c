/* $Source$
 * $State$
 * $Revision$
 */

/* Adapted from code by Chris Losinger. (This is the non-obfuscated
 * version...
 * 
 * http://www.codeproject.com/cpp/mandelbrot_obfuscation.asp
 */
 
#include <stdlib.h>
#include <unistd.h>

enum
{
	ROWS = 40,
	COLUMNS = 60,
	
	MAX_ITERATIONS = 255
};

void nl(void)
{
	write(1, "\n", 1);
}

void out(int n)
{
	const char* chars = "****++++++----  ";
	write(1, chars + (n/16), 1);
}

int main(int argc, const char* argv[])
{
    /* Size and position of the visible area. */
    
    double view_r = -2.3, view_i = -1.0;
    double zoom = 0.05;
    int x, y, n;

    for (y=0; y < ROWS; y++)
    {
        double c_i = view_i + y * zoom;
        
        for (x=0; x < COLUMNS; x++)
        {
            double c_r = view_r + x*zoom;
            double z_r = c_r;
            double z_i = c_i;
            
            for (n=0; n < MAX_ITERATIONS; n++)
            {
                double z_r2 = z_r * z_r;
                double z_i2 = z_i * z_i;

                /* Have we escaped? */
                
                if (z_r2 + z_i2 > 4)
                    break;

                /* z = z^2 + c */
                z_i = 2 * z_r * z_i + c_i;
                z_r = z_r2 - z_i2 + c_r;
            }

           	out(n);
        }
        nl();
    }
    
    return 0;
}
