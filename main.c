#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define RED   'R'
#define BLACK 'B'

typedef struct Relazione{
    char *input;
    char *output;
    char *type;
} Relazione;
typedef struct Rel{
    struct Relazione *rel;
    char color;
    struct Rel *left;
    struct Rel *right;
    struct Rel *parent;
} Rel;

typedef struct Ent{
    char *name;
    struct Rel *rel; // creo una lista per le relazioni il cui il destinatario è l'entità
    struct Rel *origine; // creo una lista per le relazioni il cui l'origine è l'entità
    struct rifRel *riferimento;
}Ent;

typedef struct Node{
    struct Ent *ent;
    char  color;
    struct Node *left;
    struct Node *right;
    struct Node *parent;
} Node;

typedef struct rifRel{
    char *rel;
    int cont;
    struct rifRel *next;
    struct rifRel *parent;
}rifRel;


typedef struct EntReport{
    char *name;
    struct EntReport * parent;
    struct EntReport *next;
}EntReport;

typedef struct Report{
    char *rel;
    int cont;
    struct EntReport *ent;
    struct Report *next;
    struct Report * parent;
}Report;



// Based on CLRS algorithm, use T_Nil as a sentinel to simplify code
struct Node  T_Nil_Node;

Node* T_Nil = &T_Nil_Node;

Node* Root = &T_Nil_Node;

struct Rel  T_Nil_Rel;
Rel* T_NilRel = &T_Nil_Rel;

int publicCount;

Ent * newEnt(char *nomepassato){
    Ent *temp= (Ent*) malloc(sizeof(Ent));
    temp->name=strdup(nomepassato);
    temp->riferimento=NULL;
    temp->rel = T_NilRel;
    temp->origine=T_NilRel;
    return temp;
}

Node* newNode(char *nomepassato)
{
    Node *temp   = (Node*) malloc(sizeof(Node));
    temp->ent=newEnt(nomepassato);
    temp->color  = RED;
    temp->left   = T_Nil;
    temp->right  = T_Nil;
    temp->parent = T_Nil;

    return temp;
}

Relazione* newRelazione(char *inputParam,char *outputParam,char *typeParam){
    Relazione *temp=(Relazione*) malloc(sizeof(Relazione));
    temp->input=strdup(inputParam);
    temp->output=strdup(outputParam);
    temp->type=strdup(typeParam);
    return temp;

}
Rel* newRel(char *inputParam,char *outputParam,char *typeParam)
{

    Rel *temp   = (Rel*) malloc(sizeof(Rel));
    temp->rel=newRelazione(inputParam,outputParam,typeParam);
    temp->color  = RED;
    temp->left   = T_NilRel;
    temp->right  = T_NilRel;
    temp->parent = T_NilRel;
    return temp;
}

rifRel* newRif(char *outParam){
    rifRel *temp= (rifRel*)malloc(sizeof(rifRel));
    temp->rel=strdup(outParam);
    temp->cont=1;
    temp->next=NULL;
    temp->parent=NULL;
    return temp;
}

EntReport* newEntReport(Node * ent){
    EntReport *temp=(EntReport*)malloc(sizeof(EntReport));
    temp->name=strdup(ent->ent->name);
    temp->next=NULL;
    temp->parent=NULL;
    return temp;
}
/*int ordineAlfabetico(char *nome1,char *nome2){
    int ris;
    ris=strcmp(nome1,nome2);
    return ris;
}*/


Report* newReport(char *rel, Node * ent){
    Report *temp=(Report*)malloc(sizeof(Report));
    int cont=1;
    while(ent->ent->riferimento!=NULL){
        if(strcmp(ent->ent->riferimento->rel,rel)==0){
            cont=ent->ent->riferimento->cont;
            break;
        }
        ent->ent->riferimento=ent->ent->riferimento->next;
    }
    if(cont==0)
        cont=1;
    temp->rel=strdup(rel);
    temp->cont=cont;
    temp->ent=newEntReport(ent);
    temp->next=NULL;
    temp->parent=NULL;
    return temp;
}



Report * rootReport=NULL;


//rotazione albero destro e sinistro per le relazioni per le entità
void rightRotateEnt(Node** T, Node* y){
    Node* x=y->left;
    y->left=x->right;
    if (x->right != T_Nil)
        x->right->parent = y;
    x->parent = y->parent;  // link y's parent to x
    if (y->parent == T_Nil)
        *T = x;
    else if (y == y->parent->right)
        y->parent->right = x;
    else
        y->parent->left  = x;
    x->right  = y;         // put y on x's right
    y->parent = x;
}

void leftRotateEnt(Node **T, Node *x) {
    Node* y;
    y=x->right;
    x->right=y->left;
    if(y->left!=T_Nil)
        y->left->parent=x;
    y->parent=x->parent;
    if(x->parent==T_Nil)
        *T=y;
    else if(x==x->parent->left)
        x->parent->left=y;
    else
        x->parent->right=y;
    y->left=x;
    x->parent=y;
}

//rotazione albero destro e sinistro per le relazioni
void rightRotateRel(Rel** T, Rel* y){
    Rel* x=y->left;
    y->left=x->right;
    if (x->right != T_NilRel)
        x->right->parent = y;
    x->parent = y->parent;  // link y's parent to x
    if (y->parent == T_NilRel)
        *T = x;
    else if (y == y->parent->right)
        y->parent->right = x;
    else
        y->parent->left  = x;
    x->right  = y;         // put y on x's right
    y->parent = x;
}

void leftRotateRel(Rel **T, Rel *x) {
    Rel* y;
    y=x->right;
    x->right=y->left;
    if(y->left!=T_NilRel)
        y->left->parent=x;
    y->parent=x->parent;
    if(x->parent==T_NilRel)
        *T=y;
    else if(x==x->parent->left)
        x->parent->left=y;
    else
        x->parent->right=y;
    y->left=x;
    x->parent=y;
}

void rbInsertFixupEnt(Node** T, Node* z){
    Node* root = *T;
    Node* x;
    Node*y;
    if(z==root)
        root->color=BLACK;
    else{
        x=z->parent;
        if(x->color==RED){
            if(x==x->parent->left){
                y=x->parent->right;
                if(y->color==RED){
                    x->color=BLACK;
                    y->color=BLACK;
                    x->parent->color=RED;
                    rbInsertFixupEnt(T,x->parent);
                }
                else {
                    if (z == x->right) {
                        z = x;
                        leftRotateEnt(T, z);
                        x = z->parent;
                    }
                    x->color = BLACK;
                    x->parent->color = RED;
                    rightRotateEnt(T, x->parent);
                }
            }
            else{
                y=x->parent->left;
                if(y->color==RED){
                    x->color=BLACK;
                    y->color=BLACK;
                    x->parent->color=RED;
                    rbInsertFixupEnt(T,x->parent);
                }
                else {
                    if (z == x->left) {
                        z = x;
                        rightRotateEnt(T, z);
                        x = z->parent;
                    }
                    x->color = BLACK;
                    x->parent->color = RED;
                    leftRotateEnt(T, x->parent);
                }
            }
        }
    }

}


void rbInsertFixupRel(Rel** R, Rel* z){
    Rel* root = *R;
    Rel* x;
    Rel*y;
    if(z==root)
        root->color=BLACK;
    else{
        x=z->parent;
        if(x->color==RED){
            if(x==x->parent->left){
                y=x->parent->right;
                if(y->color==RED){
                    x->color=BLACK;
                    y->color=BLACK;
                    x->parent->color=RED;
                    rbInsertFixupRel(R,x->parent);
                }
                else {
                    if (z == x->right) {
                        z = x;
                        leftRotateRel(R, z);
                        x = z->parent;
                    }
                    x->color = BLACK;
                    x->parent->color = RED;
                    rightRotateRel(R, x->parent);
                }
            }
            else{
                y=x->parent->left;
                if(y!=NULL){
                    if(y->color==RED){
                        x->color=BLACK;
                        y->color=BLACK;
                        x->parent->color=RED;
                        rbInsertFixupRel(R,x->parent);
                    }
                    else {
                        if (z == x->left) {
                            z = x;
                            rightRotateRel(R, z);
                            x = z->parent;
                        }
                        x->color = BLACK;
                        x->parent->color = RED;
                        leftRotateRel(R, x->parent);
                    }
                }

            }
        }
    }

}


int insertEnt(Node** T, char *nome){
    Node* y=T_Nil;
    Node* x = *T;
    int doppione;
    doppione=0;

    while(x!=T_Nil){
        y=x;
        if(strcmp(nome,x->ent->name)<0) {
            x = x->left;
        }else if(strcmp(nome,x->ent->name)==0){
            return 1;
        }else {
            x = x->right;
        }
    }
    Node* z =  newNode(nome);
    z->parent=y;
    if(y==T_Nil){
        *T=z;
    }
    else if(strcmp(z->ent->name,y->ent->name)<0){
        y->left=z;
    }else if(strcmp(z->ent->name,y->ent->name)==0){
        doppione=1;
    }
    else{
        y->right=z;
    }

    // Init z as a red leaf
    z->left  = T_Nil;
    z->right = T_Nil;
    z->color = RED;

    rbInsertFixupEnt(T,z);
    return doppione;
}



int insertRel(Rel** R, char *inParam, char *outParam, char *typeParam){
    Rel* y=T_NilRel;
    Rel* x = *R;
    int doppione;
    doppione=0;
    while(x!=T_NilRel){
        y=x;
        if(strcmp(typeParam,x->rel->type)<0) {
            x = x->left;
        }else if(strcmp(typeParam,x->rel->type)==0){
            if (strcmp(outParam, x->rel->output) < 0) {
                x = x->left;
            } else if (strcmp(outParam, x->rel->output) == 0 && strcmp(inParam, x->rel->input) == 0) {
                return 1;
            } else if (strcmp(outParam, x->rel->output) ==0) {
                if(strcmp(inParam, x->rel->input)< 0 )
                    x    = x->left;
                else
                    x=x->right;
            }
            else {
                x = x->right;
            }


        }
        else {
            x = x->right;
        }
    }
    Rel* z =  newRel(inParam,outParam,typeParam);
    z->parent=y;
    if(y==T_NilRel){
        *R=z;
    }else if(strcmp(z->rel->type,y->rel->type)==0){
        if(strcmp(z->rel->output,y->rel->output)<0){
            y->left=z;
        }else if(strcmp(z->rel->output,y->rel->output)==0){

            if(strcmp(z->rel->input,y->rel->input)<0) {
                y->left = z;
            }else if(strcmp(z->rel->input,y->rel->input)>0){
                y->right = z;
            }
            else{
                doppione=1;
            }
        }
        else{
            y->right=z;
        }
    }
    else if(strcmp(z->rel->type,y->rel->type)<0){
        y->left=z;
    }

    else {
        y->right=z;
    }

    // Init z as a red leaf
    z->left  = T_NilRel;
    z->right = T_NilRel;
    z->color = RED;

    rbInsertFixupRel(R,z);
    return doppione;
}
void aggiornaReport(Node * x, rifRel * aggRel){
    Report* new=rootReport;
    Report* testa=new;
    Report* temp=NULL;
    int is;
    is=0;
    if(new==NULL){
        temp=newReport(aggRel->rel,x);
        testa=temp;
    }else{
        while(new!=NULL) {
            if (strcmp(new->rel, aggRel->rel) == 0) {
                if (new->cont < aggRel->cont) {
                    new->cont = aggRel->cont;
                    while(new->ent!=NULL){
                        EntReport *ind=new->ent->next;
                        free(new->ent->name);
                        free(new->ent);
                        new->ent=ind;
                    }
                    free(new->ent);
                    new->ent = newEntReport(x);
                    rootReport=testa;
                    return;
                } else if (new->cont == aggRel->cont) {
                    EntReport *agg = newEntReport(x);
                    EntReport *count = new->ent;
                    agg->next = count;
                    agg->parent = count->parent;
                    count->parent = agg;
                    count = agg;

                    while (count != NULL) {
                        if (count->next == NULL) {
                            break;
                        } else if (strcmp(count->next->name, count->name) < 0) {
                            EntReport *scambio = count;
                            EntReport *next = NULL;
                            if (count->next->next != NULL)
                                next = count->next->next;
                            if(count->parent!=NULL)
                                count->parent->next=count->next;
                            count = count->next;
                            count->parent = scambio->parent;
                            count->next = scambio;
                            count->next->parent = count;
                            if (next!= NULL)
                                next->parent = count->next;
                            count->next->next=next;
                            count = count->next;
                        } else {
                            count = count->next;
                        }

                    }
                    while (count != NULL) {
                        if (count->parent == NULL) {
                            new->ent = count;
                            break;
                        }
                        count = count->parent;
                    }
                    while(new!=NULL){
                        if(new->parent==NULL){
                            testa=new;
                            break;
                        }
                        new=new->parent;
                    }
                    rootReport=testa;
                    return;
                }else{
                    if(new->next!=NULL)
                        new=new->next;
                    else{
                        is=1;
                        break;
                    }

                }
                return;
            } else{
                if(new->next!=NULL)
                    new=new->next;
                else
                    break;
            }

        }
        while(new->parent!=NULL && is==0){
            new=new->parent;
        }

        while(new!=NULL && is==0){
            temp=newReport(aggRel->rel,x);
            if(strcmp(aggRel->rel,new->rel)<0 || strcmp(aggRel->rel,new->rel)>0){
                Report * ord=new;
                temp->next=ord;
                temp->parent=ord->parent;
                ord->parent=temp;
                ord=temp;
                while(ord!=NULL){
                    if(ord->next==NULL){
                        break;
                    }
                    if(strcmp(ord->next->rel,ord->rel)<0) {
                        Report *scambio = ord;
                        Report *next = NULL;
                        if (ord->next->next != NULL) {
                            next = ord->next->next;
                        }
                        if(ord->parent!=NULL)
                            ord->parent->next=ord->next;
                        ord = ord->next;
                        ord->parent = scambio->parent;
                        ord->next = scambio;
                        ord->next->parent=ord;

                        if (next != NULL)
                            next->parent = ord->next;

                        ord->next->next = next;
                        ord = ord->next;
                    } else
                        ord=ord->next;
                }

                while(ord!=NULL){
                    if(ord->parent==NULL){
                        testa=ord;
                        break;
                    }
                    ord=ord->parent;
                }
                break;
            }else{
                new=new->next;
            }
        }

    }

    rootReport=testa;
}

rifRel* rifReport(Node * x, char *type){
    rifRel* new=(x->ent->riferimento);
    rifRel* testa=(x->ent->riferimento);
    if(new==NULL){
        new=newRif(type);;
        //aggiornaReport(x,new);
        publicCount=1;
        return new;
    }else{
        while(new!=NULL){
            if(strcmp(new->rel,type)==0){
                new->cont=new->cont+1;
                //aggiornaReport(x,new);
                publicCount=1;
                return testa;
            }
            if(new->next!=NULL){
                new=new->next;
            }
            else{
                new->next=newRif(type);
                new->next->parent=new;
                //aggiornaReport(x,new->next);
                publicCount=1;
                break;
            }
        }
    }
    return testa;


}

//serve per creare la lista delle relazioni in entrata per una determinata relazione
int collegamento(Node** T, char *outParam, char *inParam,  char *typeParam){
    Node* y=T_Nil;
    Node* x = *T;
    Node* k = *T;
    rifRel* rif;
    int doppione, is1,is2;
    doppione=0;
    is1=0;
    is2=0;
    while (x != T_Nil) {
        y = x;
        if (strcmp(outParam, x->ent->name) < 0) {
            x = x->left;
        } else if (strcmp(outParam, x->ent->name) == 0) {
            is1 = 1;
            break;
        } else {
            x = x->right;
        }
    }
    while (k != T_Nil) {
        y = k;
        if (strcmp(inParam, k->ent->name) < 0) {
            k = k->left;
        } else if (strcmp(inParam, k->ent->name) == 0) {
            is2 = 1;
            break;
        } else {
            k = k->right;
        }
    }

    if (is1 == 1 && is2 == 1 ) {
        doppione = insertRel(&(x->ent->rel), inParam, outParam, typeParam);
        if (doppione != 1) {
            rif = rifReport(x, typeParam);
            x->ent->riferimento = rif;
        }
        doppione = insertRel(&(k->ent->origine), inParam, outParam, typeParam);
    }

    return doppione;

}

//fixup delete relazioni
void rbDeleteFixupRel(Rel ** T,Rel *x) {
    Rel *w;
    if (x->color == RED || x->parent == T_NilRel) {
        x->color = BLACK;
    } else if (x == x->parent->left) {
        w = x->parent->right;

        if (w->color == RED) {
            w->color = BLACK;
            x->parent->color = RED;
            leftRotateRel(T, x->parent);
            w = x->parent->right;
        }
        if(w!=T_NilRel){
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                rbDeleteFixupRel(T, x->parent);
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    rightRotateRel(T, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                leftRotateRel(T, x->parent);
            }
        }

    } else {
        w = x->parent->left;

        if (w->color == RED) {
            w->color = BLACK;
            x->parent->color = RED;
            rightRotateRel(T, x->parent);
            w = x->parent->left;
        }
        if(w!=T_NilRel){
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                rbDeleteFixupRel(T, x->parent);
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    leftRotateRel(T, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                rightRotateRel(T, x->parent);
            }
        }

    }
}

Rel * treeMinRel(Rel  * x){
    while(x->left != T_NilRel){
        x=x->left;
    }
    return x;
}

Rel * treeSuccessorRel(Rel * x){
    Rel * y;
    if(x!=T_NilRel)
        return treeMinRel(x->right);
    y= x->parent;
    while(y != T_NilRel && x==y->right){
        x=y;
        y=y->parent;
    }
    return y;
}

void delRel(Rel ** R, Rel * z){
    Rel *y;
    Rel *x;
    if(z->left == T_NilRel || z->right==T_NilRel){
        y=z;
    }else{
        y=treeSuccessorRel(z);
    }
    if(y->left!=T_NilRel){
        x=y->left;
    }else if(y->right!=T_NilRel) {
        x=y->right;
    }else{
        x=T_NilRel;
    }
    if(x!=T_NilRel)
        x->parent = y->parent;
    if (y->parent == T_NilRel) {
        *R = x;
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }
    if (y != z) {
        z->rel=y->rel;
    }
    if (y->color == BLACK ) {
        rbDeleteFixupRel(R, x);
    }
    free(y);
}


void aggiornaReport2(Node * x){
    rifRel * y=x->ent->riferimento;
    while(y!=NULL) {
        aggiornaReport(x,y);
        if(y->next!=NULL)
            y=y->next;
        else
            break;
    }
    while(y->parent!=NULL){
        y=y->parent;
    }
    x->ent->riferimento=y;
}

void visit(Node * t){

    if(t==T_Nil||t==NULL){
        return;
    }
    else{
        visit(t->left);
        if(t->ent->riferimento!=NULL)
            aggiornaReport2(t);
        visit(t->right);
    }
}

void cancellaReport(Node * t){
    Report *temp=rootReport;
    int i=0;
    while(temp!=NULL){
        Report * scambio=NULL;
        if(temp->next!=NULL){
            scambio=temp->next;
            temp->next->parent=temp->next;
            scambio->parent=temp->parent;
            i=1;
        }
        EntReport *del=temp->ent;
        while(del!=NULL){
            EntReport *cambio=del->next;
            if(cambio!=NULL){
                cambio->parent=del->parent;
                del->parent=cambio;
            }
            free(del->name);
            free(del);
            del=cambio;
        }
        free(temp->rel);
        free(temp);
        temp=scambio;
    }
    rootReport=temp;
    visit(t);
}



int controlloMassimo(char *outParam, char *typeParam){
    Report *temp=rootReport;
    int count;
    count=0;
    while(temp!=NULL){
        if(strcmp(temp->rel,typeParam)==0){
            EntReport *i=temp->ent;
            if(strcmp(i->name,outParam)==0 && i->next==NULL){
                count++;
            }else{
                while(i!=NULL){
                    if(strcmp(i->name,outParam)==0){
                        EntReport * scambio=i->next;
                        if(scambio!=NULL){
                            if(i->parent==NULL){
                                scambio->parent=NULL;
                            }else{
                                scambio->parent=i->parent;
                                i->parent->next=scambio;
                            }
                        }
                        if(i->parent!=NULL)
                            i->parent->next=scambio;
                        scambio=i->next;
                        free(i->name);
                        free(i);
                        i=scambio;
                        break;
                    }
                    i=i->next;
                }
            }
            if(i!=NULL){
                while(i->parent!=NULL){
                    i=i->parent;
                }
                temp->ent=i;
            }
        }
        temp=temp->next;
    }
    return count;
}

void aggRiferimento(Node * x, char *typeParam){
    rifRel * temp=x->ent->riferimento;
    while(temp!=NULL){
        if(strcmp(temp->rel,typeParam)==0){
            temp->cont--;
            if(temp->cont==0){
                rifRel *scambio=temp->next;
                if(scambio!=NULL){
                    scambio->parent=temp->parent;
                }
                if(temp->parent!=NULL){
                    temp->parent->next=scambio;
                }
                rifRel *second=temp->parent;
                free(temp->rel);
                free(temp);
                if(scambio!=NULL)
                    temp=scambio;
                else
                    temp=second;
                if(temp!=NULL) {
                    while (temp->parent != NULL) {
                        temp = temp->parent;
                    }
                }
                x->ent->riferimento=temp;
                break;
            }
        }
        temp=temp->next;
    }
}
int cercaRelazione(Rel ** R, Node * ent, char *inParam, char *outParam, char *typeParam,int tipo){
    Rel *x=*R;
    Rel *y=T_NilRel;
    Rel *z;
    int count;
    count=0;
    while(x!=T_NilRel) {
        y = x;
        if (strcmp(typeParam, x->rel->type) < 0) {
            x = x->left;
        } else if (strcmp(typeParam, x->rel->type) == 0) {
            if (strcmp(outParam, x->rel->output) < 0) {
                x = x->left;
            } else if (strcmp(outParam, x->rel->output) == 0 && strcmp(inParam, x->rel->input) == 0) {
                z = x;
                if(tipo==1){
                    aggRiferimento(ent,typeParam);
                    count=controlloMassimo(outParam, typeParam);
                }

                Relazione * temp=z->rel;
                delRel(R,z);
                free(temp->input);
                free(temp->output);
                free(temp->type);
                free(temp);

                break;
            } else if (strcmp(outParam, x->rel->output) ==0) {
                if(strcmp(inParam, x->rel->input)< 0 )
                    x    = x->left;
                else
                    x=x->right;
            }
            else {
                x = x->right;
            }
        }else {
            x = x->right;
        }
    }
    return count;
}


void deleteRelazione(Node** T, char *inParam, char *outParam, char *typeParam){
    Node *x=*T;
    Node *k=*T;
    Node *y=T_Nil;
    int count;
    while(x!=T_Nil){
        y=x;
        if(strcmp(outParam,x->ent->name)<0) {
            x = x->left;
        }else if(strcmp(outParam,x->ent->name)==0){
            count=cercaRelazione(&(x->ent->rel),x,inParam,outParam,typeParam,1);


            if(count!=0)
                publicCount=1;
            //rbDeleteRel(&(x->rel),inParam,outParam,typeParam);
            break;
        }
        else {
            x = x->right;
        }
    }
    while(k!=T_Nil){
        y=k;
        if(strcmp(inParam,k->ent->name)<0) {
            k = k->left;
        }else if(strcmp(inParam,k->ent->name)==0){
            count=cercaRelazione(&(k->ent->origine),k,inParam,outParam,typeParam,2);
            //rbDeleteRel(&(k->origine),inParam,outParam,typeParam);
            break;
        }
        else {
            k = k->right;
        }
    }

}

//fixup delete enttià
void rbDeleteFixupEnt(Node ** T,Node *x){
    Node * w;
    if(x->color==RED || x->parent==T_Nil){
        x->color=BLACK;
    }else if(x==x->parent->left){
        w=x->parent->right;
        if(w->color==RED){
            w->color=BLACK;
            x->parent->color=RED;
            leftRotateEnt(T,x->parent);
            w=x->parent->right;
        }
        if(w!=T_Nil){
            if(w->left->color==BLACK  && w->right->color==BLACK){
                w->color=RED;
                rbDeleteFixupEnt(T,x->parent);
            }else{
                if(w->right->color==BLACK){
                    w->left->color=BLACK;
                    w->color=RED;
                    rightRotateEnt(T,w);
                    w=x->parent->right;
                }
                w->color=x->parent->color;
                x->parent->color=BLACK;
                leftRotateEnt(T,x->parent);
            }
        }
    }else {
        w = x->parent->left;
        if (w->color == RED) {
            w->color = BLACK;
            x->parent->color = RED;
            rightRotateEnt(T, x->parent);
            w = x->parent->left;
        }
        if(w!=T_Nil){
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                rbDeleteFixupEnt(T, x->parent);
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    leftRotateEnt(T, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                rightRotateEnt(T, x->parent);
            }
        }
    }
}

Node * treeMinEnt(Node  * x){
    while(x->left != T_Nil){
        x=x->left;
    }
    return x;
}

Node * treeSuccessorEnt(Node * x){
    Node * y;
    if(x->right!=T_Nil)
        return treeMinEnt(x->right);
    y= x->parent;
    while(y != T_Nil && x==y->right){
        x=y;
        y=y->parent;
    }
    return y;
}

void cambiaRiferimento(Node *x){
    Report * temp=rootReport;
    while(temp!=NULL){
        if(temp->ent->next==NULL) {
            if (temp->ent->name == x->ent->name) {
                temp->ent->name = x->ent->name;
            }
        }else{
            EntReport* i=temp->ent;
            while(i!=NULL){
                if(i->name==x->ent->name){
                    i->name=x->ent->name;
                }
                i=i->next;
            }
        }
        temp=temp->next;
    }

}


void delEnt(Node **T, Node *z){
    Node *y;
    Node *x;
    if(z->left == T_Nil || z->right==T_Nil){
        y=z;
    }else{
        y=treeSuccessorEnt(z);
    }
    if(y->left!=T_Nil){
        x=y->left;
    }else{
        x=y->right;
    }
    if(x!=T_Nil)
        x->parent=y->parent;
    if(y->parent==T_Nil){
        *T=x;
    }else if(y==y->parent->left){
        y->parent->left=x;
    }else{
        y->parent->right=x;
    }
    if(y!=z){
        z->ent=y->ent;
        cambiaRiferimento(z);
    }
    if(y->color==BLACK){
        rbDeleteFixupEnt(T,x);
    }
    free(y);
}


int delRelAltraEntita(Node** T, Rel * g,int type){
    Node *x=*T;
    Node *k=*T;
    Node *y=T_Nil;
    int count;
    if(type==1){
        while(x!=T_Nil){
            if(strcmp(g->rel->output,x->ent->name)<0) {
                x = x->left;
            }else if(strcmp(g->rel->output,x->ent->name)==0){
                count=cercaRelazione(&(x->ent->rel),x,g->rel->input,g->rel->output,g->rel->type,1);
                break;
            }
            else {
                x = x->right;
            }
        }
    }else if(type!=1){
        while(x!=T_Nil){
            if(strcmp(g->rel->input,x->ent->name)<0) {
                x = x->left;
            }else if(strcmp(g->rel->input,x->ent->name)==0){
                count=cercaRelazione(&(x->ent->origine),x,g->rel->input,g->rel->output,g->rel->type,2);
                break;
            }
            else {
                x = x->right;
            }
        }
    }
    return count;

}

int cancellaRelazione(Rel **R, Node *x,Node ** T,int type){
    Rel * y=*R;
    int count;
    count=delRelAltraEntita(T,y,type);
    if(type==2){
        aggRiferimento(x,y->rel->type);
        //count=controlloMassimo(y->rel->output, y->rel->type);
    }

    Relazione * temp=y->rel;
    delRel(R,y);
    free(temp->input);
    free(temp->output);
    free(temp->type);
    free(temp);

    y=*R;
    if(y!=T_NilRel)
        count=count+cancellaRelazione(R,x,T,type);
    return count;
}

void deleteEntita(Node ** T, char *nome){
    Node *x=*T;
    Node *y=T_Nil;
    Node *z;
    int count1=0;
    int count2=0;
    while(x!=T_Nil){
        y=x;
        if(strcmp(nome,x->ent->name)<0) {
            x = x->left;
        }else if(strcmp(nome,x->ent->name)==0){
            z=x;
            if(z->ent->origine!=T_NilRel){
                count1=cancellaRelazione(&z->ent->origine,z,T,1);
            }
            if(z->ent->rel!=T_NilRel){
                count2=cancellaRelazione(&z->ent->rel,z,T,2);
            }
            Ent * temp=z->ent;
            delEnt(T,z);
            //free(temp->origine);
//            free(temp->riferimento->rel);
            rifRel * cancella=temp->riferimento;
            while(cancella!=NULL){
                rifRel * seguo=cancella->next;
                free(cancella->rel);
                free(cancella);
                cancella=seguo;
            }
            if(temp->rel!=T_NilRel){
                free(temp->rel);
            }
            if(temp->origine!=T_NilRel){
                free(temp->origine);
            }
            free(temp->riferimento);
            free(temp->name);
            cancellaReport(*T);
            publicCount=0;
            break;
        }
        else {
            x = x->right;
        }
    }

}

void printReport(){
    Report * new=rootReport;
    EntReport* temp=NULL;
    if(new==NULL){
        fputs("none",stdout);
    }else{
        while(new!=NULL){
            fputs("\"",stdout);
            fputs(new->rel,stdout);
            fputs("\" ",stdout);
            temp=new->ent;
            while(temp!=NULL){
                fputs("\"",stdout);
                fputs(temp->name,stdout);
                fputs("\" ",stdout);
                temp=temp->next;
            }
            if(new->next==NULL){
                printf("%d;", new->cont);
            }
            else{
                printf("%d; ", new->cont);
            }

            new=new->next;
        }
    }
    printf("\n");
}




int main() {
    char buf[200];
    char otherString[7];
    char *res;

    publicCount=0;
     //freopen("input.txt","r",stdin);
    //freopen("ris.txt","w",stdout);

    T_Nil->right=T_Nil;
    T_Nil->left=T_Nil;
    T_Nil->parent=T_Nil;
    T_Nil->color=BLACK;
    T_NilRel->left=T_NilRel;
    T_NilRel->right=T_NilRel;
    T_NilRel->parent=T_NilRel;
    T_NilRel->color=BLACK;
    while((res=fgets(buf, 200, stdin)) != NULL) {

        if( res==NULL )
            break;
        strncpy(otherString, res, 6);
        otherString[6] = '\0';

        if((strncmp(otherString, "addent", 6))==0){
            insertEnt(&Root,strtok(res+8,"\""));
        }else if((strncmp(otherString,"delent",6))==0){
            deleteEntita(&Root,strtok(res+8,"\""));
        }
        else if((strncmp(otherString, "addrel", 6))==0) {
            char *input=strtok(res+8,"\" \"");
            char *output=strtok(NULL,"\" \"");
            char *type=strtok(NULL,"\" \"");
            collegamento(&Root,output ,input ,type );
        }else if((strncmp(otherString,"delrel",6))==0){
            char *input=strtok(res+8,"\" \"");
            char *output=strtok(NULL,"\" \"");
            char *type=strtok(NULL,"\" \"");
            deleteRelazione(&Root, input, output, type);
        }else if((strncmp(otherString,"report",6))==0){

            if(publicCount==1){
                cancellaReport(Root);
                publicCount=0;
            }
            printReport();


        }else if((strncmp(otherString,"end",6))==0){
            break;
        }


    }
    /*  printTreeEnt(Root);
      printf("\n \n");
      printTreeRel(RootRel);
      printf("\n addent: %d \n",contaddent);
      printf("\n addrel: %d \n",contaddrel);
      printf("\n delent: %d \n",contadelent);
      printf("\n delrel: %d \n",contadelrel);

      printf("\n ci sono %d doppioni nelle entità \n",doppioneent);
      printf("\n ci sono %d doppioni nelle relazioni delle entità \n",doppionerelEnt);
      printf("\n ci sono %d doppioni nelle relazioni \n",doppionerel);

      printReport();*/


    return 0;
}

