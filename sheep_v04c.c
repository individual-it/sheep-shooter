#include <conio.h>
#include <stdio.h>
#include <graphics.h>
#include <stdlib.h>
#include <bios.h>
#include <dos.h>
#include <time.h>
#include <string.h>
#include <math.h>


struct namen_zeit
{
	char namen[21];
   int zeit;
};


int bewegung_zielk(void far *explo,void far *schaf,int *, int *, int *, int *,
                   int *, int *,int *, int *, int *, int *, int *);
void paint_zielkreuz(int *, int *);
void bewegung_schaf(int *,int *, int *, int *,int *, int *,int *,int *, int *, int *);
void paint_schaf(void);
void zielkreuz_uebermalen(int,int);
void schaf_plazieren(int *,int *);
void Start_Anim(void);
void highscore(int,int, int);
void paint_explo(void);
void kopf_explo(int,int);
void shit_explo(int,int);


int main(void)
{
	const kills_bis_gewonnen=10; //Anzahl der kills die erreicht werden muß bis
   									  //man gewonnen hat
   int weite,ende,gesammt_zeit;
   time_t start_zeit,end_zeit;

   int ziel_k_x=320,ziel_k_y=240,ziel_k_x_alt,ziel_k_y_alt,schaf_x,schaf_y;
   int schaf_x_alt,schaf_y_alt,richtung,kills=0, schaf_step=2;
   int shit_x,shit_y, shit_zaeler,wartezeit,zeitbonus=0;
   char kills_s[3];

   /* request autodetection */
   int gdriver = DETECT, gmode, errorcode;

   /* initialize graphics and local variables */
   initgraph(&gdriver, &gmode, "");

   /* read result of initialization */
   errorcode = graphresult();
   if (errorcode != grOk) {   /* an error occurred */
      printf("Graphics error: %s\n", grapherrormsg(errorcode));

      printf("Press any key to halt:");
      getch();
      exit(1);               /* terminate with an error code */
   }


   unsigned int size = imagesize(0,0,46,28);
   void far *schaf= malloc(size);
   void far *schaf_back= malloc(size);
   getimage(320,240,320+46,240+28,schaf_back); //Hintergrund in "schaf_back" speichern
   paint_schaf(); //Schaf einmall zeichnen
	getimage(320,240,320+46,240+28,schaf); //Schaf in "schaf" speichern

   unsigned int size_explo = imagesize(0,0,115,125);
   void far *explo= malloc(size_explo);
   paint_explo();
   getimage(0,0,115,125,explo);
   cleardevice();


   Start_Anim();

   randomize();
   schaf_plazieren(&schaf_x,&schaf_y);
   start_zeit=time(NULL);  //Ermitteln der Startzeit
   while(ende!=1)
   {
   	richtung=random(9);
	   weite=random(20);
      wartezeit=30;

      if (kills>=kills_bis_gewonnen)
      {
      	end_zeit=time(NULL); //Endzeit ermitteln
         gesammt_zeit=end_zeit-start_zeit;
         cleardevice();

         //highscore ausgeben
         highscore(gesammt_zeit,kills_bis_gewonnen,zeitbonus);

         ende=1; //Spiel beenden
      }
	   while (weite>0&&ende!=1)
	   {

         shit_zaeler--;

         //Den Dreck aufräumen den das Schaf gemacht hat
         if (shit_zaeler==0)
         {
            setcolor(0);         //Farbe auf schwarz setzen
            setfillstyle(0,0);
            circle(shit_x,shit_y,6);
            floodfill (shit_x,shit_y,0);
            wartezeit=15;
         }



         itoa(kills,kills_s, 10); //von int nach char konvertieren
         outtextxy(1,1,"kills:");
         outtextxy(48,1,kills_s);
         ende=bewegung_zielk(explo,schaf,&schaf_x,&schaf_y,&ziel_k_x,&ziel_k_y,
         						  &ziel_k_x_alt,&ziel_k_y_alt,&kills,&schaf_step,
                             &shit_x, &shit_y, &zeitbonus);
         bewegung_schaf(&schaf_x,&schaf_y,&schaf_x_alt,&schaf_y_alt,&richtung,
                        &schaf_step,&weite,&shit_x,&shit_y, &shit_zaeler);
         delay(wartezeit);
         setcolor(0);         //Farbe auf schwarz setzen
         setfillstyle(0,0);
         putimage(schaf_x_alt,schaf_y_alt, schaf_back, 0);
         putimage(schaf_x,schaf_y, schaf, 0);
         zielkreuz_uebermalen(ziel_k_x_alt,ziel_k_y_alt);
         paint_zielkreuz(&ziel_k_x,&ziel_k_y);  //Zielkreuz zeichnen
	   	weite--;


       }
   }

   closegraph();
   return 0;
}

 //Zielkreuz uebermalen
void zielkreuz_uebermalen(int ziel_k_x_alt, int ziel_k_y_alt)
{
   setcolor(0);
   circle(ziel_k_x_alt,ziel_k_y_alt,8);
   circle(ziel_k_x_alt,ziel_k_y_alt,10);
   setlinestyle(4,0xAAAA,1);
   line(ziel_k_x_alt,ziel_k_y_alt-15,ziel_k_x_alt,ziel_k_y_alt+15);
   line(ziel_k_x_alt-15,ziel_k_y_alt,ziel_k_x_alt+15,ziel_k_y_alt);
}


void paint_zielkreuz(int *ziel_k_x,int *ziel_k_y) //Zielkreuz zeichnen
{
   setcolor(4);
   circle(*ziel_k_x,*ziel_k_y,8);
   circle(*ziel_k_x,*ziel_k_y,10);
   setlinestyle(4,0xAAAA,1);
   line(*ziel_k_x,*ziel_k_y-15,*ziel_k_x,*ziel_k_y+15);
   line(*ziel_k_x-15,*ziel_k_y,*ziel_k_x+15,*ziel_k_y);
}


int bewegung_zielk(void far *explo,void far *schaf,int *schaf_x, int *schaf_y,int *ziel_k_x,
						 int *ziel_k_y,int *ziel_k_x_alt, int *ziel_k_y_alt,int *kills,
                   int *schaf_step, int *shit_x, int *shit_y, int *zeitbonus)
{
   int ende=0,getroffen_rumpf=0,getroffen_kopf=0;
   char eingabe;
   if (kbhit())
   {
     eingabe=getch();
   }
   *ziel_k_x_alt=*ziel_k_x;
   *ziel_k_y_alt=*ziel_k_y;
   switch (eingabe)
   		{
         case	'6':
         	*ziel_k_x=*ziel_k_x+3;
            break;
          case	'2':
            *ziel_k_y=*ziel_k_y+3;
            break;
          case	'4':
         	*ziel_k_x=*ziel_k_x-3;
            break;
          case	'8':
            *ziel_k_y=*ziel_k_y-3;
           	break;
         case	'3':
         	*ziel_k_x=*ziel_k_x+3;
            *ziel_k_y=*ziel_k_y+3;
            break;
         case	'1':
         	*ziel_k_x=*ziel_k_x-3;
            *ziel_k_y=*ziel_k_y+3;
            break;
         case	'7':
         	*ziel_k_x=*ziel_k_x-3;
            *ziel_k_y=*ziel_k_y-3;
            break;
         case	'9':
         	*ziel_k_x=*ziel_k_x+3;
            *ziel_k_y=*ziel_k_y-3;
            break;
         case ' ':   //Schießen

         	//Wenn der Rumpf getroffen wurde
         	if (*ziel_k_x>=*schaf_x+6&&*ziel_k_x<=*schaf_x+30&&
            	 *ziel_k_y>=*schaf_y+6&&*ziel_k_y<=*schaf_y+22)
               {
               putimage(*ziel_k_x-52,*ziel_k_y-62,explo,0);
               delay(1000);
            	*kills=*kills+1;
               cleardevice();
               schaf_plazieren(schaf_x,schaf_y);
               *schaf_step=2;
               }


            //Kopfschuß
         	if (*ziel_k_x>=*schaf_x+30&&*ziel_k_x<=*schaf_x+42&&
            	 *ziel_k_y>=*schaf_y+1&&*ziel_k_y<=*schaf_y+12)
                {

                 settextstyle(0,0,5);
                 setcolor(0);
                 setfillstyle(0,0);
                 zielkreuz_uebermalen(*ziel_k_x,*ziel_k_y);
                 putimage(*schaf_x,*schaf_y,schaf,0);
                 fillellipse(*schaf_x+40,*schaf_y+9,3,3); //Kopf übermalen
                 fillellipse(*schaf_x+34,*schaf_y+7,6,6); //Nase übermalen
                 kopf_explo(*schaf_x,*schaf_y);
                 setcolor(4);
                 outtextxy(10,10,"!!!Headshot!!!");
                 delay(1000);
              	  *kills=*kills+1;
                 cleardevice();
                 schaf_plazieren(schaf_x,schaf_y);
                 settextstyle(0,0,1);
                 *schaf_step=2;
                }


            //Wenn ein Bein getroffen wurde wird das Schaf langsammer
            if (*ziel_k_x>=*schaf_x+5&&*ziel_k_x<=*schaf_x+12&&
            	 *ziel_k_y>=*schaf_y+23&&*ziel_k_y<=*schaf_y+28)
                {
                  *schaf_step=*schaf_step-1;
                  cleardevice();
                }

            if (*ziel_k_x>=*schaf_x+24&&*ziel_k_x<=*schaf_x+28&&
            	 *ziel_k_y>=*schaf_y+23&&*ziel_k_y<=*schaf_y+28)
                {
                  *schaf_step=*schaf_step-1;
                  cleardevice();
                }


           //Wenn die Scheiße vom Schaf getroffen wird
           if (*ziel_k_x>=*shit_x-6&&*ziel_k_x<=*shit_x+6&&
              *ziel_k_y>=*shit_y-6&&*ziel_k_y<=*shit_y+6)
                {
                *zeitbonus=*zeitbonus+1;
                shit_explo(*shit_x,*shit_y);
                paint_zielkreuz(ziel_k_x,ziel_k_y);  //Zielkreuz zeichnen
                setcolor(4);
                outtextxy(100,10,"!!!Zeitbonus!!!");
                delay(500);
                setcolor(0);
                bar(100,20,220,10);

                }
            break;

          case 'q':
          	ende=1;
            break;
         }

   //Das Zielkreuz darf nicht aus dem Bildschirm rauslaufen
   if (*ziel_k_y < 0+5) *ziel_k_y=0+5;
	if (*ziel_k_y > 480-5) *ziel_k_y=480-5;
	if (*ziel_k_x < 0+5) *ziel_k_x=0+5;
	if (*ziel_k_x > 640-5) *ziel_k_x=640-5;
   return ende;
}

void bewegung_schaf(int *schaf_x,int *schaf_y,int *schaf_x_alt,int *schaf_y_alt,
				        int *richtung, int *schaf_step, int *weite, int *shit_x,
                    int *shit_y, int *shit_zaeler)
{
	*schaf_x_alt=*schaf_x;
   *schaf_y_alt=*schaf_y;
   switch (*richtung)
   {
   	case 0:
      	*schaf_x=*schaf_x+*schaf_step;
      	break;
   	case 1:
      	*schaf_y=*schaf_y+*schaf_step;
         break;
   	case 2:
      	*schaf_x=*schaf_x-*schaf_step;
         break;
     	case 3:
      	*schaf_y=*schaf_y-*schaf_step;
         break;
      case 4:
      	*schaf_x=*schaf_x+*schaf_step;
         *schaf_y=*schaf_y+*schaf_step;
         break;
      case 5:
        	*schaf_x=*schaf_x-*schaf_step;
         *schaf_y=*schaf_y+*schaf_step;
         break;
      case 6:
        	*schaf_x=*schaf_x+*schaf_step;
         *schaf_y=*schaf_y-*schaf_step;
         break;
      case 7:
      	*schaf_x=*schaf_x+*schaf_step;
         *schaf_y=*schaf_y-*schaf_step;
         break;
      case 8:             //Das Schaf macht einen Haufen
         //Den alten Haufen aufräumen
            setcolor(0);         //Farbe auf schwarz setzen
            setfillstyle(0,0);
            circle(*shit_x,*shit_y,6);
            floodfill (*shit_x,*shit_y,0);

         *weite=20;       //Damit das Schaf wartet
         *richtung=9;
         break;
      case 9:            //Schaf wartet
        *weite=*weite-1;
         if (*weite<=2) *richtung=10;
         break;
      case 10:           //Schaf macht das Geschäft
         setfillstyle(1,6);
         setcolor (6);
         circle(*schaf_x-10,*schaf_y+26,2);
         floodfill (*schaf_x-10,*schaf_y+25,6);
         circle(*schaf_x-5,*schaf_y+26,2);
         floodfill (*schaf_x-5,*schaf_y+25,6);
         circle(*schaf_x-7,*schaf_y+23,2);
         floodfill (*schaf_x-7,*schaf_y+23,6);
         *shit_x=*schaf_x-7;
         *shit_y=*schaf_y+25;
         *shit_zaeler=random(50);
         *shit_zaeler=*shit_zaeler+120 ;
         break;

     }

   //Wenn das Schaf aus dem Bildschirm rausläuft läuft es am anderem Ende wieder
   //rein
   if (*schaf_y < 0) *schaf_y=480-28;
	if (*schaf_y > 480-28) *schaf_y=0;
   if (*schaf_x < 0) *schaf_x=640-46;
	if (*schaf_x > 640-46) *schaf_x=0;
}

void paint_schaf(void)
{
	int x=320,y=240;

   setfillstyle(1,15);
	setcolor (7);
	setfillstyle(1,6);

	/*bein_hinten*/ellipse (x+10,y+21,0,360,2,7);
	floodfill (x+10,y+21,7);
	/*bein_vorne*/ellipse (x+26,y+21,0,360,2,7);
	floodfill (x+26,y+21,7);
	setfillstyle(1,15);
	/*Schwanz*/fillellipse(x+2,y+13,2,2);
	setfillstyle(1,12);
	/*Nase*/fillellipse(x+40,y+9,3,3);
	setfillstyle(1,15);
	/*Kopf*/fillellipse (x+34,y+7,6,6);
	/*rumpf*/fillellipse (x+18,y+13,15,9);
	/*auge*/putpixel(x+36,y+5,8);
	/*Ohr*/fillellipse(x+33,y+5,1,3);
}


//Schaf zufällig irgendwo plazieren
void schaf_plazieren(int *schaf_x,int *schaf_y)
{
	*schaf_x=random(640-46);
   *schaf_y=random(480-28);
}


void Start_Anim(void)
{
 //start animation
   setcolor(4);
   setlinestyle(0,0,3);
   settextstyle(0, 0,4);
   circle(320, 240,160);
   circle(320, 240,180);
   setlinestyle(4,0xAAAA,3);
   line(320,0,320,480);
   line(0,240,640,240);
   moveto (120,225);
   setcolor(1);
   outtext ("Sheep Shooter");
   moveto (125,230);
   setcolor(9);
   outtext ("Sheep Shooter");
   moveto (280,330);
   settextstyle(0, 0,1);
   setcolor(8);
   outtextxy (520,470, "Beta Ver. 0.4c");

	//Blinkender Text anzeigen
   while (!kbhit())
   {
   setcolor(8);
   moveto (280,330);
   outtext ("Press a key");
   delay(500);
   setcolor(7);
   moveto (280,330);
   outtext ("Press a key");
   delay(500);
   }
   cleardevice();
   getch();
}


void highscore(int gebrauchte_zeit, int kills_bis_gewonnen, int zeitbonus)
{
	FILE *datei;
   namen_zeit in[11];
   char namen[20],zeit_gebraucht_s[5],hilf_s[20];
   int x=0,gefunden=0,hilf;

   printf("Du hast fuer %i kills %i Sekunden gebraucht\n\n",kills_bis_gewonnen
   																      ,gebrauchte_zeit);
   gebrauchte_zeit=gebrauchte_zeit-zeitbonus;
   printf("Minus %i Sekunden Zeitbonus ergibt eine Gesammtzeit von %i Sekunden\n",
                                                         zeitbonus,gebrauchte_zeit);
   //Datei öffnen (mit Fehlerabrage)
   if ((datei=fopen("punkte.she","r"))== NULL)
   {
      fprintf(stderr,"Kann Highscoredatei nicht oeffnen.\n");
      getch();
      exit(-1);
   }


   while (x<10)
   {
      fscanf(datei,"%s",&zeit_gebraucht_s);

      if (gefunden!=1)
      {
        in[x].zeit=atoi(zeit_gebraucht_s);
         fscanf(datei,"%s",&in[x].namen);
      }
      else
      {
      	in[x+1].zeit=atoi(zeit_gebraucht_s);
         fscanf(datei,"%s",&in[x+1].namen);
      }


      if (in[x].zeit>gebrauchte_zeit&&gefunden!=1)
      {
        // strcpy(namen,"SHEEP SHOOTER");
         printf("Bitte Namen eingeben (keine Leerzeichen oder Sonderzeichen verwenden):\n");
         fgets(namen,21,stdin); //Namen einlesen max 19 Zeichen

         if (strlen(namen)<=1) strcpy(namen,"SHEEP_SHOOTER\n");

         //Zeilenumbruch löschen wenn mehr als 20 Zeichen eingegeben wurden wird
         //kein \n angehängt und deswegen wird das letzte Zeichen überschrieben
         //Durch diesen Effekt kommt die maximale von 19 zustande
         namen[strlen(namen)-1]=0;

         //Namen und Werte eins weiter verschieben
      	hilf=in[x].zeit;
         in[x].zeit=gebrauchte_zeit;
         in[x+1].zeit=hilf;
         strcpy(hilf_s,in[x].namen);
         strcpy(in[x].namen,namen);
         strcpy(in[x+1].namen,hilf_s);

         gefunden=1;
      }
       x++;
    }

    fclose(datei); //Datei schließen

    /*Datei zum schreiben öffnen (mit Fehlerabrage), die alte datei wird
    überschrieben*/
    if ((datei=fopen("punkte.she","w"))== NULL)
    {
      fprintf(stderr,"Kann Highscoredatei nicht oeffnen.\n");
      getch();
      exit(-1);
    }

   printf("Der Highscore:\n-------------\n");

 	//Namen ausgeben und in Datei schreiben
   for (x=0;x<10;x++)
   {
    printf("%s %d sec.\n",in[x].namen,in[x].zeit);
    fprintf(datei,"%d %s\n",in[x].zeit,in[x].namen);
   }
	fclose(datei); //Datei wieder schließen
   getch();
}

void paint_explo(void)
{
int explosion[40];
   setcolor(14);
   setfillstyle(1,4);
   explosion[1-1]=6;
   explosion[2-1]=23;
   explosion[3-1]=40;
   explosion[4-1]=43;
   explosion[5-1]=38;
   explosion[6-1]=12;
   explosion[7-1]=49;
   explosion[8-1]=42;
   explosion[9-1]=80;
   explosion[10-1]=11;
   explosion[11-1]=66;
   explosion[12-1]=50;
   explosion[13-1]=89;
   explosion[14-1]=45;
   explosion[15-1]=70;
   explosion[16-1]=62;
   explosion[17-1]=114;
   explosion[18-1]=64;
   explosion[19-1]=72;
   explosion[20-1]=74;
   explosion[21-1]=83;
   explosion[22-1]=100;
   explosion[23-1]=59;
   explosion[24-1]=85;
   explosion[25-1]=51;
   explosion[26-1]=122;
   explosion[27-1]=38;
   explosion[28-1]=92;
   explosion[29-1]=15;
   explosion[30-1]=111;
   explosion[31-1]=34;
   explosion[32-1]=77;
   explosion[33-1]=6;
   explosion[34-1]=73;
   explosion[35-1]=33;
   explosion[36-1]=59;
   explosion[37-1]=explosion[1-1];
   explosion[38-1]=explosion[2-1];
   fillpoly(19, explosion);
   }

void kopf_explo(int schaf_x,int schaf_y)
{
  int x[7]={0}, y[7]={0},i=0;

  while(i<10)
  {
    x[0]++;
    y[0]++;
    x[1]++;
    y[1]--;
    x[2]=x[2]+2;
    y[2]++;
    y[3]++;
    x[4]=x[4]+3;
    y[5]=y[5]-2;
    x[6]=i;
    y[6]=i+1;


    putpixel(schaf_x+28+x[0],schaf_y+5+y[0],4);
    putpixel(schaf_x+27+x[1],schaf_y+4+y[1],4);
    putpixel(schaf_x+29+x[2],schaf_y+6+y[2],4);
    putpixel(schaf_x+29+x[3],schaf_y+7+y[3],4);
    putpixel(schaf_x+30+x[4],schaf_y+5+y[4],4);
    putpixel(schaf_x+27+x[5],schaf_y+4+y[5],4);
    putpixel(schaf_x+32+x[6],schaf_y+6+y[6],4);
    putpixel(schaf_x+28+x[4],schaf_y+4+y[5],4);
    putpixel(schaf_x+30+x[4],schaf_y+6+y[6],4);
	 i++;
	 delay(10);
}

}

void shit_explo(int shit_x,int shit_y)
{
  int x[9]={0}, y[9]={0},i=0;

  while(i<15)
  {
    x[0]++;
    y[0]--;
    x[1]++;
    y[1]=sqrt(i+x[3])*-1;
    x[2]=x[2]+2;
    y[2]--;
    x[3]=sqrt(i);
    y[3]--;
    x[4]=x[4]-2;
    y[4]=sqrt(i*x[3])*-1;
    x[5]=x[2]*-1;
    y[5]=y[2];
    x[6]=i*2*-1;
    y[6]=i*sin(y[4])*-1;
    y[7]--;
    y[8]--;
    x[8]=sqrt(i*random(10))*-1;
    putpixel(shit_x+x[0],shit_y+y[0],6);
    putpixel(shit_x+1+x[1],shit_y+y[1],6);
    putpixel(shit_x+2+x[2],shit_y+3+y[2],6);
    putpixel(shit_x+3+x[3],shit_y+2+y[3],6);
    putpixel(shit_x+x[4],shit_y+1+y[4],6);
    putpixel(shit_x+1+x[5],shit_y+3+y[5],6);
    putpixel(shit_x+2+x[6],shit_y+2+y[6],6);
    putpixel(shit_x+3+x[7],shit_y+1+y[7],6);
    putpixel(shit_x+x[8],shit_y+y[8],6);
	 i++;
	 delay(10);
}
delay(50);
setcolor(0);
fillellipse(shit_x,shit_y-6,35,15);

}


