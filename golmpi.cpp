// Esperimento di crittografia usando un automa cellulare (Game of life)

#include <iostream>
#include <cmath>
#include <allegro.h>
#include <mpi.h>
#include <stdio.h>

#define master 0

using namespace std; 
  //Buffer 
 unsigned  char gola[64][64];  // PARTE ALTA 
 unsigned  char golb[64][64];  //Parte BASSA
 unsigned  char golc[64][64]; // Parte Centrale
 unsigned  char gol [64][64];  

void crypt_bmp(BITMAP *img, unsigned char *key)
 {
  unsigned char r,g,b;
  int c;
  int p = 0;

  for(int x=0;x<img->w;x++)
    {
     for(int y=0;y<img->h;y++)
      {
       c = getpixel(img,x,y);
       r = getr(c);
       g = getg(c);
       b = getb(c);
       r = ~(r ^ key[p%512]);
       g = ~(g ^ key[p%512]);
       b = ~(b ^ key[p%512]);
       p++;
       putpixel(img,x,y,makecol32(r,g,b));
      }
    }
 }

void decrypt_bmp(BITMAP *img, unsigned char *key)
 {
  unsigned char r,g,b;
  int c;
  int p = 0;

  for(int x=0;x<img->w;x++)
    {
     for(int y=0;y<img->h;y++)
      {
       c = getpixel(img,x,y);
       r = getr(c);
       g = getg(c);
       b = getb(c);
       r = ~(r ^ key[p%512]);
       g = ~(g ^ key[p%512]);
       b = ~(b ^ key[p%512]);
       p++;
       putpixel(img,x,y,makecol32(r,g,b));
      }
    }
 }

void crypt(unsigned char *txt, unsigned char *key)
 {
  int p = 0;
  for(int i = 0; i < 512; i++)
   {
	txt[i] = txt[i] + key[p%16];
	p++;
   }
 }

char getBit(unsigned char v, int n)  { return ( v >> n ) & 0x01; } // Prende il valore dell'n-esimo bit di v e lo ritorna in un carattere (0 o 1)

void aggiorna_ckey(unsigned char *key, unsigned char gol[][64])
 {
  // Aggiorna la chiave finale (di lunghezza 512 caratteri)
  int v;
  for(int i = 0; i < 512*8; i+=8)
   {
    v = 0; for(int k = 0; k < 8; k++) v += ( 3*pow(2,7-k) * (gol[(i%64)+k][i/64]) );
    key[i/8] = (unsigned char)(key[i/8] - v);
   }
 }

void CreaStatoIniziale(unsigned char gol[][64],unsigned char key[],int gen)
 {
  // Crea lo stato iniziale della matrice di game of life a partire dalla chiave e dal numero di generazioni
  // metteremo valore 0 per cella morta, 1 per cella attiva
  for(int y = 0; y < 64; y++)
   for(int x = 0; x < 64; x++)
	{
	 // Distribuisce la chiave in maniera non troppo banale
	 gol[x][y] = 0;
	 gol[x][y] = getBit(key[ ( y + x + gen / (x*y + 1) ) % 16],( ( x*y + gen / (x + y + 1) ) + key[x/4] ) % 8);
    }
 }

void printGrid(BITMAP *buf)
 {
  int col;
  for(int v = 0; v < 64; v++)
   {
	if(v % 8 == 0) col = makecol32(0,255,255); else col = makecol32(120,120,120);
    vline(buf, v * 10,      0, SCREEN_H, col);
    hline(buf,      0, v * 10, SCREEN_W, col);
   }
 }

void printMatrix(unsigned char gol[][64], int gen, BITMAP *buf)
 {
  for(int y = 0; y < 64; y++)
   for(int x = 0; x < 64; x++)
	{
	 if(gol[x][y] == 1) rectfill(buf,x*10,y*10,x*10+10,y*10+10,makecol32(255,255,255));
    }
  printGrid(buf);
  textprintf_ex(buf,font,2,2,makecol32(255,0,0),-1,"Generazione %d",gen);
  sync();
  
 }

int gol_count(int x, int y, unsigned char gol[][64])
 {
  // Conta i "vicini" della cella x,y
  int xt,yt,c;

  c = 0;
  for(int xx = -1; xx < 2; xx++)
   for(int yy = -1; yy < 2; yy++)
    {
	 xt = x + xx;
	 yt = y + yy;
	 if(xt >= 0 && xt < 64 && yt >=0 && yt < 64) if(xt != x && yt != y) c += gol[xt][yt];
	}
  return c;
 }

void gol_step(unsigned char gol[][64])
 {
  char tmp[64][64];
  int c;

  // Effettua uno step
  for(int y = 0; y < 64; y++)
   for(int x = 0; x < 64; x++)
	{
	 c = gol_count(x,y,gol);
	 if(gol[x][y] == 1) // Cella "viva"
	  {
       // Each cell with one or no neighbors dies, as if by solitude.
       // Each cell with four or more neighbors dies, as if by overpopulation.
       // Each cell with two or three neighbors survives.
       tmp[x][y] = 1;
	   if(c <= 1) tmp[x][y] = 0;
	   if(c >= 4) tmp[x][y] = 0;
	  }
	 else // Cella "morta"
	  {
	   // Each cell with three neighbors becomes populated.
	   tmp[x][y] = 0;
	   if(c == 3) gol[x][y] = 1;
	  }
    }

  // Copia la nuova matrice che è in tmp in gol
  for(int y = 0; y < 64; y++)
   for(int x = 0; x < 64; x++)
	{
	 gol[x][y] = tmp[x][y];
    }
 }


//gol_step utilizzato dal nodo myid=1

void gol_step_1(unsigned char gol[][64])
 {
 
 //gola 
 
  int c;

  // Effettua uno step
  for(int y = 0; y < 24; y++)
   for(int x = 0; x < 64; x++)
	{
	 c = gol_count(x,y,gol);
	 if(gol[x][y] == 1) // Cella "viva"
	  {
       // Each cell with one or no neighbors dies, as if by solitude.
       // Each cell with four or more neighbors dies, as if by overpopulation.
       // Each cell with two or three neighbors survives.
       gola[x][y] = 1;
	   if(c <= 1) gola[x][y] = 0;
	   if(c >= 4) gola[x][y] = 0;
	  }
	 else // Cella "morta"
	  {
	   // Each cell with three neighbors becomes populated.
	   gola[x][y] = 0;
	   if(c == 3) gol[x][y] = 1;
	  }
    } 
 }



 //gol_step utilizzato dal nodo myid=2
void gol_step_2(unsigned char gol[][64])
  {
   
   int c;

   // Effettua uno step
   for(int y = 40; y < 64; y++)
    for(int x = 0; x < 64; x++)
 	{
 	 c = gol_count(x,y,gol);
 	 if(gol[x][y] == 1) // Cella "viva"
 	  {
        // Each cell with one or no neighbors dies, as if by solitude.
        // Each cell with four or more neighbors dies, as if by overpopulation.
        // Each cell with two or three neighbors survives.
        golb[x][y] = 1;
 	   if(c <= 1) golb[x][y] = 0;
 	   if(c >= 4) golb[x][y] = 0;
 	  }
 	 else // Cella "morta"
 	  {
 	   // Each cell with three neighbors becomes populated.
 	   golb[x][y] = 0;
 	   if(c == 3) gol[x][y] = 1;
 	  }
     }

}

  //gol_step utilizzato dal nodo myid=3

  void gol_step_3(unsigned char gol[][64])
   {
     int c;
     //golc
    // Effettua uno step
    for(int y = 24; y < 40; y++)
     for(int x = 0; x < 64; x++)
  	{
  	 c = gol_count(x,y,gol);
  	 if(gol[x][y] == 1) // Cella "viva"
  	  {
         // Each cell with one or no neighbors dies, as if by solitude.
         // Each cell with four or more neighbors dies, as if by overpopulation.
         // Each cell with two or three neighbors survives.
         golc[x][y] = 1;
  	   
  	   if(c <= 1) golc[x][y] = 0;
  	   if(c >= 4) golc[x][y] = 0;
  	  }
  	 else // Cella "morta"
  	  {
  	   // Each cell with three neighbors becomes populated.
  	   golc[x][y] = 0;
  	   if(c == 3) gol[x][y] = 1;
  	  }
      }
 
   }

 void gol_sum()
  {
     
     //glo a 
     // step 1
   for(int y = 0; y < 24; y++) {
    for(int x = 0; x < 64; x++) gol[x][y]=gola[x][y];
   }
    //gol b
     //  step 2
   for(int y = 40; y <64; y++) {
    for(int x =0; x <64; x++) gol[x][y] = golb[x][y];
   }
   
    // gol c 
    //  step 3
    for(int y = 24; y < 40; y++) {
     for(int x = 0; x < 64; x++) gol[x][y] = golc[x][y];
    }

}


 int main (int argc, char *argv[])
 {
   int err, nproc, myid;
   MPI_Status status;
   
   int  generations,gen; 
   
   char fname[255]; // Nome file immagine
   
   
   unsigned char key[16],ckey[512]; // la grandezza di ckey dipende dalle dimensioni della matrice gol ed è dim(gol)/8
 
  

  err = MPI_Init(&argc, &argv);
  err = MPI_Comm_size(MPI_COMM_WORLD, &nproc);
  err = MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  if(myid==0)
  {

  cout << "Inserisci 16 caratteri: ";
  cin >> key;
  cout << "Inserisci il numero di generazioni (max 255 min 2): ";
  cin >> generations;
  cout << "Inserisci nome file: (bitmap 24 bpp): ";
  cin >> fname;

  cout << "\n\nChiave di base: " << key << "\nGenerazioni: " << generations;

  gen = 0;
  CreaStatoIniziale(gol,key,generations);

  allegro_init();
  install_keyboard();
  set_color_depth(32);
  if(set_gfx_mode(GFX_AUTODETECT_WINDOWED,640,640,0,0)) { cout << "Errore video\n"; return -1; }

  clear_to_color(screen,makecol32(0,0,0)); 

}


 MPI_Bcast (&gen,1, MPI_INT, master, MPI_COMM_WORLD );
 MPI_Bcast (&generations,1, MPI_INT, master, MPI_COMM_WORLD );


 
// Generazioni   

  while(gen < generations)
   {
	 
	 if (myid==master){  
		 
	   printMatrix(gol,gen,screen); 
       
	 }

  //mandiamo la matrice a tutti i nodi
     
    MPI_Bcast (&gol,64*64, MPI_CHAR, master, MPI_COMM_WORLD );
 
 /* i nodi mandano il loro contributo computazionale */
 
     if (myid!=master) { 
 
        if (myid==1) {
			gol_step_1(gol);
			err = MPI_Send(&gola,64*64, MPI_CHAR, master, 10, MPI_COMM_WORLD);
        }
        
        if (myid==2){
			 gol_step_2(gol);
			 err = MPI_Send(&golb,64*64, MPI_CHAR, master, 10, MPI_COMM_WORLD);
        }
        
        if (myid==3) {
			gol_step_3(gol);
			err = MPI_Send(&golc,64*64, MPI_CHAR, master, 10, MPI_COMM_WORLD);
			
        }
         
       
     } // i nodi hanno mandato i dati
 
     else  
     {  
       // Nodo Master 
    
      /* 
       Il nodo Master riceve i dati, e somma i contributi
        computazionali di tutti i nodi.
      */
    
      MPI_Recv ( &gola, 64*64, MPI_CHAR, 1, 10,MPI_COMM_WORLD, &status);
      MPI_Recv ( &golb, 64*64, MPI_CHAR, 2, 10,MPI_COMM_WORLD, &status);
      MPI_Recv ( &golc, 64*64, MPI_CHAR, 3, 10,MPI_COMM_WORLD, &status);
    
      gol_sum();  
      
      
        clear_to_color(screen,makecol32(0,0,0));
        printMatrix(gol,gen,screen); // Matrice finale
      
      // e aggiorna i dati 
       aggiorna_ckey(ckey,gol);
	   gen++;
	 
	
   }
    
      MPI_Bcast (&gen,1, MPI_INT, master, MPI_COMM_WORLD ); 
       
      
       printf("gen: %d - nodo %d\n",gen,myid);
         
      
     }  // while
	 

// il nodo master Presenta i risultati


 if (myid==master) {
  clear_to_color(screen,makecol32(0,0,0));
  printMatrix(gol,gen,screen); // Matrice finale
  readkey();

  crypt(ckey,key);     // Ottengo la chiave finale combinando quella iniziale (16 byte) con ckey
  cout << "\n Chiave finale (512 byte): \n" ; for(int j=0;j<512;j++) printf("%0x ",ckey[j]);

  // Con la chiave ckey cripto un'immagine bitmap
  cout << "\nCarico " << fname << endl;
  BITMAP *img = load_bmp(fname,NULL); // Carica il file ".bmp"
  clear_to_color(screen,makecol32(0,0,0)); draw_sprite(screen,img,0,0); readkey();
  crypt_bmp(img,ckey);
  clear_to_color(screen,makecol32(0,0,0)); draw_sprite(screen,img,0,0); readkey();
  decrypt_bmp(img,ckey);
  clear_to_color(screen,makecol32(0,0,0)); draw_sprite(screen,img,0,0); readkey();
	
 }
	 
 
  err=MPI_Finalize();
  return 0;
 
 } END_OF_MAIN()
