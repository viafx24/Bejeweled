// Jeux sans grand int�r�t, on passera vite.
// mais a l'air quand m�me hardu dans le code. regardons Cela.
// le but est d'aligner trois symboles en hauteur ou largeur qui disparaissent alors
// et retombe par le haut des nouveaux symboles aleatoires. Il peut y avoir des 
// evenements en cascade.

// update avant commit final:
// tout est loin d'�tre clair mais j'ai l'image global. je bloc l�. Seul les points d'algotrithmique necessiterait d'�tre eclairci.
// les points de programmation c++ sont clair et c'est cela qui importe.


#include <SFML/Graphics.hpp>
#include <time.h>
using namespace sf;

int ts = 54; //tile size (des gems)
Vector2i offset(48,24); // correspond au bord de l'image ajout�e


// cette ecriture m'a gen� avec le grid[10][10] . Il s'agit bien de la declaration d'une structure de type piece en tableau bidimensionnel.
// Elle existe certainement en memoire imm�diatement (mais je n'ai pas verifi�).
struct piece
{ int x,y,col,row,kind,match,alpha;
  piece(){match=0; alpha=255;}
} grid[10][10];// je pense qu'on initialise un tableau bidimensionnel de 10 par 10 piece (comme d'hab vec plus de place car semble 8*8)


// on notera ici deux fonctions swap: une qui est la notre , une qui appartient � la std et donc on rajoute le std::
void swap(piece p1,piece p2)
{
  std::swap(p1.col,p2.col);// intervertit les deux objets simplement.
  std::swap(p1.row,p2.row);

  // je n'ai pas regard� pr�cisement le pourquoi mais il faut sans doute r�indiqu� dans la grille qui est qui.
  grid[p1.row][p1.col]=p1;
  grid[p2.row][p2.col]=p2;
}


int main()
{
    srand(time(0));

    RenderWindow app(VideoMode(740,480), "Match-3 Game!");
    app.setFramerateLimit(60);

    Texture t1,t2;
    t1.loadFromFile("images/background.png");
    t2.loadFromFile("images/gems.png");

    Sprite background(t1), gems(t2);

// assez etrange, la colonne et ligne 0 et 9 ne sont pas rempli et l'esp�ce (kind) est compris entre 0 et 3 or il y a 6 types
// cela reste coherent dans la mesure ou le tableau est un 8*8
// il est possible que la taille 10*10 de la grille serve quand on parcourt avec n-1 et n+1 � ne pas depasser la taille du tableau.
// une astuce � garder en t�te.

    for (int i=1;i<=8;i++)
     for (int j=1;j<=8;j++)
      {
          grid[i][j].kind=rand()%3; // bizarre car semble avoir 6 type
          grid[i][j].col=j;// en colonne (travail avec colonne et ligne commen�ant � 1 et pas zero)
          grid[i][j].row=i;
          grid[i][j].x = j*ts; // en pixel
          grid[i][j].y = i*ts;
      }

    int x0,y0,x,y; int click=0; Vector2i pos;
    bool isSwap=false, isMoving=false;

    while (app.isOpen())
    {
        Event e;
        while (app.pollEvent(e))
        {
            if (e.type == Event::Closed)
                app.close();
                   
            if (e.type == Event::MouseButtonPressed)
                if (e.key.code == Mouse::Left)
                {
                   if (!isSwap && !isMoving) click++; // ne marche que si on n'est pas en train de bouger ou d'�changer
                   pos = Mouse::getPosition(app)-offset; // on recup�re la position de la souris (soustraction de vecteur j'imagine)
                }
         }
    
   // mouse click
   if (click==1)
    {
      x0=pos.x/ts+1;//enregistre la position du premier click
      y0=pos.y/ts+1;
    }
   if (click==2)
    {
      x=pos.x/ts+1;// enregistre la position du second click
      y=pos.y/ts+1;

      // verifie si les deux cases sont adjacentes (horizontalement ou verticalement mais pas diagonalement (2)
      if (abs(x-x0)+abs(y-y0)==1) 
          // tente un swap des deux cases en appelant la fonction, reminitialise click et met le booleen isSwap � 1
        {swap(grid[y0][x0],grid[y][x]); isSwap=1; click=0;}
      else click=1;// sinon consid�re que le deuxi�me click est un premier click (s'arr�tera dans le if du dessus pour updater x0 et
      // y0 au temps suivant.
    }

   //Match finding
   // dans les lignes qui suivent, on recherche simplement un match 3 verticalement ou horizontalement 
   // et on incremente match pour les cellules repondant � la condition.

   for(int i=1;i<=8;i++)// 1 car le -1 qui suit peut donner l'indice 0
   for(int j=1;j<=8;j++)// idem
   {
    if (grid[i][j].kind==grid[i+1][j].kind)
    if (grid[i][j].kind==grid[i-1][j].kind)
     for(int n=-1;n<=1;n++) grid[i+n][j].match++;// si il y a match des lignes, incr�menter match pour les 3 cases en lignes impliqu�es

    if (grid[i][j].kind==grid[i][j+1].kind)// idem pour les colonnes
    if (grid[i][j].kind==grid[i][j-1].kind)
     for(int n=-1;n<=1;n++) grid[i][j+n].match++;
   }

   //Moving animation
   isMoving=false;
   for (int i=1;i<=8;i++)
    for (int j=1;j<=8;j++)
     {
       piece &p = grid[i][j];
       int dx,dy;
       for(int n=0;n<4;n++)   // 4 - speed
       {dx = p.x-p.col*ts;
        dy = p.y-p.row*ts;
        if (dx) p.x-=dx/abs(dx);
        if (dy) p.y-=dy/abs(dy);}
       if (dx||dy) isMoving=1;
     }

   //Deleting amimation
   // on a l'impression que c'est de la partie dessin mais il n'en est rien.
   // si on est sur une case avec un match > 0, alors d�croit alpha progressivement jusqu'� une transparence tr�s faible ?
   // un fois � un alpha de 10, la condition isMoving repasse � false avec la ligne au dessus (au temps suivant)

   // que si n'est pas en train de bouger. en fait juste apr�s le swap (mais ca bouge plus)
   // si une case a un match >1, la transparence alpha diminue progressivement jusqu � 0 (>10 m�ne � zero je pense)
   // et on consid�re que isMoving repasse � true car un effet se produit mais je je comprends alors pas encore pourquoi
   // on rentr dedans avec le !ismoving

   if (!isMoving)
    for (int i=1;i<=8;i++)
    for (int j=1;j<=8;j++)
    if (grid[i][j].match) if (grid[i][j].alpha>10) {grid[i][j].alpha-=10; isMoving=true;}

   //Get score
   int score=0;
   for (int i=1;i<=8;i++)
    for (int j=1;j<=8;j++)
      score+=grid[i][j].match;// le score est simplement l'incrementation de la variable match pour toute les cellules
   // je me demandais pourquoi match n'�tait pas un booleen, j'ai ma r�ponse. Contrairement � ce qu'on pourrait penser
   // score ne sera pas utilis� pour afficher le score mais r�iniatilis� en permanence � zero. il serait facile cependant
   // d'ajouter le score en cr�ant une deuxi�me variable sur le cot�.

   //Second swap if no match
   if (isSwap && !isMoving)
       // si il y a eu swap mais que il ne doit pas y avoir echange
       // on verifie si le score est nul et on refait un swap ce qui revient � l'inverser. on repasse le swap � zero
       // il faudra tester ca en debug
       // attention, score est r�intialis� � chaque temps (il ne montre pas le score � la fin dans la fen�tre d'ailleurs)

      {if (!score) swap(grid[y0][x0],grid[y][x]); isSwap=0;}

   //Update grid
   // si n'est pas en train de bouger j'imagine

   if (!isMoving)
    {
      for(int i=8;i>0;i--)
       for(int j=1;j<=8;j++)
         if (grid[i][j].match)

             // c'est une histoire de: si tu tombe sur un match>0, alors toute les lignes du dessus se decalent vers le bas
             // et cela n'a lieu qu'avec les lignes car ca marche pareil qu'on est 3 match vertical ou horizontal

             // i correspond � la taille de la ligne: se rappeler que la taille des lignes augmente en descendant d'o� le sens inverse du for
         for(int n=i;n>0;n--)
             //le but, je pense est de faire "remonter" le grid qui match jusqu'� ce qu'il se retrouve en position zero invisible.
             // cela donne le sentiment avec les swap successif que toutes les cellules sont descendus. Par contre, je ne
             // comprends pas le break et donc mon explication est peut-�tre (sans doute) fausse.
            if (!grid[n][j].match) {swap(grid[n][j],grid[i][j]); break;};

      // recr�ation al�atoire (cette fois on a bien un 7 et pas un trois)
      // a rebosser.

      for(int j=1;j<=8;j++)
       for(int i=8,n=0;i>0;i--)
         if (grid[i][j].match) // pour tout match, il faudra r�cr�er un nouveau symbole
           {
            grid[i][j].kind = rand()%7; // un vrai cette fois (la premi�re fois il n'y en a que trois mais on a pas le temps de le voir)
            grid[i][j].y = -ts*n++; //le y et c'est ce que j'ai du mal � me mettre dans le cr�ne correspond au ligne ( et x au colonne) (en g�n�ral c'est l'inverse)
            // ici -ts sort de l'�cran et c'est effectivement ce qu'on voit pour les nouveaux symboles: il ne depasse qu'une petite partie du bas
            grid[i][j].match=0;
            grid[i][j].alpha = 255;
           }
     }


    //////draw///////
    app.draw(background);

    for (int i=1;i<=8;i++)
     for (int j=1;j<=8;j++)
      {
        piece p = grid[i][j]; // ligne � bosser
        gems.setTextureRect( IntRect(p.kind*49,0,49,49));
        gems.setColor(Color(255,255,255,p.alpha));
        gems.setPosition(p.x,p.y);
        gems.move(offset.x-ts,offset.y-ts); // ligne � bosser
        app.draw(gems);
      }

     app.display();
    }
    return 0;
}
