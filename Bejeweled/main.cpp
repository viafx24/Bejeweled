// Jeux sans grand intêrêt, on passera vite.
// mais a l'air quand même hardu dans le code. regardons Cela.
// le but est d'aligner trois symboles en hauteur ou largeur qui disparaissent alors
// et retombe par le haut des nouveaux symboles aleatoires. Il peut y avoir des 
// evenements en cascade.

// update avant commit final:
// tout est loin d'être clair mais j'ai l'image global. je bloc là. Seul les points d'algotrithmique necessiterait d'être eclairci.
// les points de programmation c++ sont clair et c'est cela qui importe.


#include <SFML/Graphics.hpp>
#include <time.h>
using namespace sf;

int ts = 54; //tile size (des gems)
Vector2i offset(48,24); // correspond au bord de l'image ajoutée


// cette ecriture m'a gené avec le grid[10][10] . Il s'agit bien de la declaration d'une structure de type piece en tableau bidimensionnel.
// Elle existe certainement en memoire immédiatement (mais je n'ai pas verifié).
struct piece
{ int x,y,col,row,kind,match,alpha;
  piece(){match=0; alpha=255;}
} grid[10][10];// je pense qu'on initialise un tableau bidimensionnel de 10 par 10 piece (comme d'hab vec plus de place car semble 8*8)


// on notera ici deux fonctions swap: une qui est la notre , une qui appartient à la std et donc on rajoute le std::
void swap(piece p1,piece p2)
{
  std::swap(p1.col,p2.col);// intervertit les deux objets simplement.
  std::swap(p1.row,p2.row);

  // je n'ai pas regardé précisement le pourquoi mais il faut sans doute réindiqué dans la grille qui est qui.
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

// assez etrange, la colonne et ligne 0 et 9 ne sont pas rempli et l'espèce (kind) est compris entre 0 et 3 or il y a 6 types
// cela reste coherent dans la mesure ou le tableau est un 8*8
// il est possible que la taille 10*10 de la grille serve quand on parcourt avec n-1 et n+1 à ne pas depasser la taille du tableau.
// une astuce à garder en tête.

    for (int i=1;i<=8;i++)
     for (int j=1;j<=8;j++)
      {
          grid[i][j].kind=rand()%3; // bizarre car semble avoir 6 type
          grid[i][j].col=j;// en colonne (travail avec colonne et ligne commençant à 1 et pas zero)
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
                   if (!isSwap && !isMoving) click++; // ne marche que si on n'est pas en train de bouger ou d'échanger
                   pos = Mouse::getPosition(app)-offset; // on recupère la position de la souris (soustraction de vecteur j'imagine)
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
          // tente un swap des deux cases en appelant la fonction, reminitialise click et met le booleen isSwap à 1
        {swap(grid[y0][x0],grid[y][x]); isSwap=1; click=0;}
      else click=1;// sinon considère que le deuxième click est un premier click (s'arrêtera dans le if du dessus pour updater x0 et
      // y0 au temps suivant.
    }

   //Match finding
   // dans les lignes qui suivent, on recherche simplement un match 3 verticalement ou horizontalement 
   // et on incremente match pour les cellules repondant à la condition.

   for(int i=1;i<=8;i++)// 1 car le -1 qui suit peut donner l'indice 0
   for(int j=1;j<=8;j++)// idem
   {
    if (grid[i][j].kind==grid[i+1][j].kind)
    if (grid[i][j].kind==grid[i-1][j].kind)
     for(int n=-1;n<=1;n++) grid[i+n][j].match++;// si il y a match des lignes, incrémenter match pour les 3 cases en lignes impliquées

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
   // si on est sur une case avec un match > 0, alors décroit alpha progressivement jusqu'à une transparence trés faible ?
   // un fois à un alpha de 10, la condition isMoving repasse à false avec la ligne au dessus (au temps suivant)

   // que si n'est pas en train de bouger. en fait juste aprés le swap (mais ca bouge plus)
   // si une case a un match >1, la transparence alpha diminue progressivement jusqu à 0 (>10 mène à zero je pense)
   // et on considère que isMoving repasse à true car un effet se produit mais je je comprends alors pas encore pourquoi
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
   // je me demandais pourquoi match n'était pas un booleen, j'ai ma réponse. Contrairement à ce qu'on pourrait penser
   // score ne sera pas utilisé pour afficher le score mais réiniatilisé en permanence à zero. il serait facile cependant
   // d'ajouter le score en créant une deuxième variable sur le coté.

   //Second swap if no match
   if (isSwap && !isMoving)
       // si il y a eu swap mais que il ne doit pas y avoir echange
       // on verifie si le score est nul et on refait un swap ce qui revient à l'inverser. on repasse le swap à zero
       // il faudra tester ca en debug
       // attention, score est réintialisé à chaque temps (il ne montre pas le score à la fin dans la fenètre d'ailleurs)

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

             // i correspond à la taille de la ligne: se rappeler que la taille des lignes augmente en descendant d'où le sens inverse du for
         for(int n=i;n>0;n--)
             //le but, je pense est de faire "remonter" le grid qui match jusqu'à ce qu'il se retrouve en position zero invisible.
             // cela donne le sentiment avec les swap successif que toutes les cellules sont descendus. Par contre, je ne
             // comprends pas le break et donc mon explication est peut-être (sans doute) fausse.
            if (!grid[n][j].match) {swap(grid[n][j],grid[i][j]); break;};

      // recréation aléatoire (cette fois on a bien un 7 et pas un trois)
      // a rebosser.

      for(int j=1;j<=8;j++)
       for(int i=8,n=0;i>0;i--)
         if (grid[i][j].match) // pour tout match, il faudra récréer un nouveau symbole
           {
            grid[i][j].kind = rand()%7; // un vrai cette fois (la première fois il n'y en a que trois mais on a pas le temps de le voir)
            grid[i][j].y = -ts*n++; //le y et c'est ce que j'ai du mal à me mettre dans le crâne correspond au ligne ( et x au colonne) (en général c'est l'inverse)
            // ici -ts sort de l'écran et c'est effectivement ce qu'on voit pour les nouveaux symboles: il ne depasse qu'une petite partie du bas
            grid[i][j].match=0;
            grid[i][j].alpha = 255;
           }
     }


    //////draw///////
    app.draw(background);

    for (int i=1;i<=8;i++)
     for (int j=1;j<=8;j++)
      {
        piece p = grid[i][j]; // ligne à bosser
        gems.setTextureRect( IntRect(p.kind*49,0,49,49));
        gems.setColor(Color(255,255,255,p.alpha));
        gems.setPosition(p.x,p.y);
        gems.move(offset.x-ts,offset.y-ts); // ligne à bosser
        app.draw(gems);
      }

     app.display();
    }
    return 0;
}
