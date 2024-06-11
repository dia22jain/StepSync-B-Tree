#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define SIZE 100
#define WEEK 7
#define MAX_LINE_LENGTH 200
#define MAX_INDIVIDUALS 100 // Assuming a maximum number of individuals in the file
#define MAX_IND_GRP 5

typedef enum{FAILURE,SUCCESS} status_code;

#define MAX_KEYS 4
#define MIN_KEYS 2
#define ORDER 5

typedef struct Individual
{
   int ind_id;
   char name[SIZE];
   int age;
   int daily_step_goal;
   int step_count[WEEK];
   int group_id;
} Individual;

typedef struct Member
{
   int mem_id;
   Individual* mem_ptr;
}Member;   

typedef struct Group
{
   int grp_id;
   char grp_name[SIZE];
   Member member[MAX_IND_GRP];
   int weekly_group_goal;
}Group;

typedef struct Copy_Group
{
   int grp_id;
   char grp_name[SIZE];
   int total_steps;
}Copy_Group;

typedef struct btree_node_individual{
    Individual individual[ORDER-1];
    struct btree_node_individual *ptrs[ORDER];
    int size;
    struct btree_node_individual *parent;
}ind;

typedef struct btree_node_group{
    Group group[ORDER-1];
    struct btree_node_group *ptrs[ORDER];
    int size;
    struct btree_node_group *parent;
}grp;

typedef struct btree_node_cpygroup{
    Copy_Group group[ORDER-1];
    struct btree_node_cpygroup *ptrs[ORDER];
    int size;
    struct btree_node_cpygroup *parent;
}cpygrp;

typedef struct Top_3_node
{
    int ind_ID;
    char name[SIZE];
    int steps;
} Top_3;


static int individual_id=0;        //ID generator for individual
static int group_id=0;             //ID generator for group


ind *ind_root=NULL;                
grp *grp_root=NULL;

void Copy(Individual* ptr1,Individual* ptr2)            //This function copies all the information of individual from ptr2 to ptr1
{
    //printf("%d\n",ptr2->ind_id);
    ptr1->ind_id = ptr2->ind_id;
    strcpy(ptr1->name,ptr2->name);
    ptr1->age=ptr2->age;
    ptr1->daily_step_goal=ptr2->daily_step_goal;
    ptr1->group_id=ptr2->group_id;
    for(int i=0;i<WEEK;i++)
    {
         ptr1->step_count[i]=ptr2->step_count[i];
    }
}

ind* create_ind() 
{
    ind*new_node = (ind*)malloc(sizeof(ind));
    if (new_node != NULL) {
        new_node->size = 0;
        for(int i = 0; i < ORDER; i++) {
            new_node->ptrs[i] = NULL;
        }
    }
    new_node->parent = NULL;
    return new_node;
}

ind* split(ind* root,int data_id,char data_name[],int data_age,int data_grp_id,int data_dsp,int data_step_count[])
{
    //printf("SPLIT\n");
     int i = 0;
    Individual Array[ORDER];
    while(i < (ORDER - 1) && root->individual[i].ind_id < data_id)
    {
          //printf("While 1\n");
          Copy(&(Array[i]),&(root->individual[i]));
          i++;
    }
    //printf("While 1 completed\n");
    Array[i].ind_id = data_id;
    strcpy(Array[i].name,data_name);
    Array[i].age=data_age;
    Array[i].daily_step_goal = data_dsp;
    Array[i].group_id=data_grp_id;
    for(int j=0;j<WEEK;j++)
            Array[i].step_count[j]=data_step_count[j];
    i++;
    //printf("Before while 2\n");
    //printf("%d %d",i,ORDER);
    while(i < (ORDER))
    {
          //printf("While 2\n");
          Copy(&(Array[i]),&(root->individual[i-1]));
          i++;
    }
    /*for(i=0;i<ORDER;i++)
    {
        printf("%d ",Array[i]);
    }*/
    int median_index = (ORDER-1)/2;
    //printf("median_index = %d\n",median_index);
     if(root->parent == NULL)
     {
          //printf("parent is NULL\n");
          ind* newnode = create_ind();
          ind* parent = create_ind();
          
          Copy(&(parent->individual[0]),&(Array[median_index]));
          parent->size = 1;
          parent->ptrs[0]=root;
          for(i=0;i<median_index;i++)
          {
                Copy(&(root->individual[i]),&(Array[i]));
          }
          root->size = median_index;
          int k = 0;
          for(i=median_index + 1;i< ORDER;i++)
          {
                Copy(&(newnode->individual[k]),&(Array[i]));
                k++;
          }
          newnode->size = k;
          parent->ptrs[1]=newnode;
          i = ORDER - 1;
          k=newnode->size;
          while(i > median_index + 2)
          {
               newnode->ptrs[k] = root->ptrs[i];
               k--;
               if(root->ptrs[i]!= NULL)
                   root->ptrs[i]->parent = newnode;
               i--;
          }
          if( root->ptrs[median_index + 2] != NULL)
          {
               ind *extra = create_ind();
               root->ptrs[median_index + 2]->size = median_index;
               root->ptrs[median_index + 2]->parent = newnode;
               newnode->ptrs[k] = extra;
               newnode->ptrs[k - 1] = root->ptrs[median_index + 2];
               int k = 0;
               for(i=median_index ;i< ORDER - 1;i++)
               {
                   Copy(&(extra->individual[k]),&(root->ptrs[median_index + 2]->individual[i]));
                   k++;
               }
               extra->size = k;
               extra->parent = newnode;
               newnode->ptrs[0] = root->ptrs[median_index + 1];
               if(root->ptrs[median_index]!=NULL)
               root->ptrs[median_index + 1]->parent = newnode;
          }
          
          
          
          root->parent=parent;
          newnode->parent=parent;
          ind_root=parent;
          //printf("%p\n",parent);
          return parent;
      }
      
      else
      {
          if(root->parent->size == ORDER - 1 )
          {
                //printf("If the parent node is full\n");
                for(i=0;i<median_index;i++)
                { 
                     Copy(&(root->individual[i]),&(Array[i]));
                }
                for(i=median_index+1;i<ORDER;i++)
                {
                    Copy(&(root->individual[i-1]),&(Array[i]));
                }
                /*for(i=0;i<ORDER - 1;i++)
                {
                     printf("%d ",root->Data[i]);
                }*/
                root = split(root->parent, Array[median_index].ind_id, Array[median_index].name, Array[median_index].age, Array[median_index].group_id, Array[median_index].daily_step_goal,Array[median_index].step_count);
                
          }
          else
          {
                //i = root->parent->size - 1;
                ind* newnode = create_ind();
                if(newnode == NULL)
                {
                     printf("MEMORY ALLOCATION FAILED\n");
                     return root;
                }
                for(i=0;i<median_index;i++)
                {
                     Copy(&(root->individual[i]),&(Array[i]));
                }
                root->size = median_index;
                int k = 0;
                for(i=median_index+1;i<ORDER;i++)
                {
                     Copy(&(newnode->individual[k]),&(Array[i]));
                     k++;
                }
                newnode->size = k;
                // If the parent node is not full, insert the value into this node
                i = root->parent->size - 1;
                while (i >= 0 && root->parent->individual[i].ind_id > Array[median_index].ind_id) {
                       Copy(&(root->parent->individual[i+1]),&(root->parent->individual[i]));
                       root->parent->ptrs[i + 2] = root->parent->ptrs[i+1];
                       i--;
                }
                Copy(&(root->parent->individual[i+1]),&(Array[median_index]));
                root->parent->ptrs[i + 2] = newnode;
                newnode->parent = root->parent;
                root->parent->size++;
          }
     }
     /*while(root->parent!=NULL)
                {
                    root=root->parent;
                }*/
     return root;
}
                

ind* insert(ind *root,int data_id,char data_name[],int data_age,int data_grp_id,int data_dsp,int data_step_count[]) {
    

    if (root == NULL) {
        root = create_ind();
        if (root == NULL) {
            
        } 
        else {
            root->individual[0].ind_id = data_id;
            strcpy(root->individual[0].name,data_name);
            root->individual[0].age=data_age;
            root->individual[0].daily_step_goal = data_dsp;
            root->individual[0].group_id=data_grp_id;
            for(int i=0;i<WEEK;i++)
                root->individual[0].step_count[i]=data_step_count[i];
            root->size = 1;
        }
    } 
    else {
        if(root->ptrs[0] == NULL)//no child
        {
           if(root->size == ORDER - 1)//nodes is full we,need to split :(
           {
           
               root = split(root,data_id,data_name,data_age,data_grp_id,data_dsp,data_step_count);
               
           }
           else
           {
                int i = root->size - 1;
                while (i >= 0 && root->individual[i].ind_id > data_id) {
                    Copy(&(root->individual[i+1]),&(root->individual[i]));
                    i--;
                }
                root->individual[i+1].ind_id = data_id;
                strcpy(root->individual[i+1].name,data_name);
                root->individual[i+1].age=data_age;
                root->individual[i+1].daily_step_goal = data_dsp;
                root->individual[i+1].group_id=data_grp_id;
                for(int j=0;j<WEEK;j++)
                     root->individual[i+1].step_count[j]=data_step_count[j];
                root->size++;
           }
        }
        else
        {
            int i = root->size - 1;
            while (i >= 0 && root->individual[i].ind_id > data_id) {
               i--;
            }
            root->ptrs[i + 1] = insert((root->ptrs[i + 1]), data_id,data_name,data_age,data_grp_id,data_dsp,data_step_count);
             
         }
     }
     
     return root;
}               

grp* create_grp() 
{
    grp* new_node = (grp*)malloc(sizeof(grp));
    if (new_node != NULL) {
        new_node->size = 0;
        for(int i = 0; i < ORDER; i++) {
            new_node->ptrs[i] = NULL;
        }
    }
    new_node->parent = NULL;
    return new_node;
}

void Copy_grp(Group* ptr1,Group* ptr2)
{
    //printf("%d\n",ptr2->ind_id);
    ptr1->grp_id = ptr2->grp_id;
    strcpy(ptr1->grp_name,ptr2->grp_name);
    ptr1->weekly_group_goal=ptr2->weekly_group_goal;
    for(int i=0;i<MAX_IND_GRP;i++)
    {
         ptr1->member[i].mem_id=ptr2->member[i].mem_id;
         ptr1->member[i].mem_ptr=ptr2->member[i].mem_ptr;
    }
}

grp* split_grp(grp* root,int data_id,char data_name[],int data_wgg,Member array[])
{
    //printf("SPLIT\n");
     int i = 0;
    Group Array[ORDER];
    while(i < (ORDER - 1) && root->group[i].grp_id < data_id)
    {
          //printf("While 1\n");
          Copy_grp(&(Array[i]),&(root->group[i]));
          i++;
    }
    //printf("While 1 completed\n");
    Array[i].grp_id = data_id;
    strcpy(Array[i].grp_name,data_name);
    Array[i].weekly_group_goal = data_wgg;
    for(int j=0;j<MAX_IND_GRP;j++)
    {
            Array[i].member[j].mem_id=array[j].mem_id;
            Array[i].member[j].mem_ptr=array[j].mem_ptr;
    }
    i++;
    while(i < (ORDER))
    {
          Copy_grp(&(Array[i]),&(root->group[i-1]));
          i++;
    }
    int median_index = (ORDER-1)/2;
    //printf("median_index = %d\n",median_index);
     if(root->parent == NULL)
     {
          grp* newnode = create_grp();
          grp* parent = create_grp();
          
          Copy_grp(&(parent->group[0]),&(Array[median_index]));
          parent->size = 1;
          parent->ptrs[0]=root;
          for(i=0;i<median_index;i++)
          {
                Copy_grp(&(root->group[i]),&(Array[i]));
          }
          root->size = median_index;
          int k = 0;
          for(i=median_index + 1;i< ORDER;i++)
          {
                Copy_grp(&(newnode->group[k]),&(Array[i]));
                k++;
          }
          newnode->size = k;
          parent->ptrs[1]=newnode;
          i = ORDER - 1;
          k=newnode->size;
          while(i > median_index + 2)
          {
               newnode->ptrs[k] = root->ptrs[i];
               k--;
               if(root->ptrs[i]!= NULL)
                   root->ptrs[i]->parent = newnode;
               i--;
          }
          if( root->ptrs[median_index + 2] != NULL)
          {
               grp *extra = create_grp();
               root->ptrs[median_index + 2]->size = median_index;
               root->ptrs[median_index + 2]->parent = newnode;
               newnode->ptrs[k] = extra;
               newnode->ptrs[k - 1] = root->ptrs[median_index + 2];
               int k = 0;
               for(i=median_index ;i< ORDER - 1;i++)
               {
                   Copy_grp(&(extra->group[k]),&(root->ptrs[median_index + 2]->group[i]));
                   k++;
               }
               extra->size = k;
               extra->parent = newnode;
               newnode->ptrs[0] = root->ptrs[median_index + 1];
               if(root->ptrs[median_index]!=NULL)
               root->ptrs[median_index + 1]->parent = newnode;
          }
          
          
          
          root->parent=parent;
          newnode->parent=parent;
          grp_root=parent;
          return parent;
      }
      
      else
      {
          if(root->parent->size == ORDER - 1 )
          {
                for(i=0;i<median_index;i++)
                { 
                     Copy_grp(&(root->group[i]),&(Array[i]));
                }
                for(i=median_index+1;i<ORDER;i++)
                {
                    Copy_grp(&(root->group[i-1]),&(Array[i]));
                }
                root = split_grp(root->parent, Array[median_index].grp_id, Array[median_index].grp_name, Array[median_index].weekly_group_goal, Array[median_index].member);
                
          }
          else
          {
                //i = root->parent->size - 1;
                grp* newnode = create_grp();
                if(newnode == NULL)
                {
                     printf("MEMORY ALLOCATION FAILED\n");
                     return root;
                }
                for(i=0;i<median_index;i++)
                {
                     Copy_grp(&(root->group[i]),&(Array[i]));
                }
                root->size = median_index;
                int k = 0;
                for(i=median_index+1;i<ORDER;i++)
                {
                     Copy_grp(&(newnode->group[k]),&(Array[i]));
                     k++;
                }
                newnode->size = k;
                // If the parent node is not full, insert the value into this node
                i = root->parent->size - 1;
                while (i >= 0 && root->parent->group[i].grp_id > Array[median_index].grp_id) {
                       Copy_grp(&(root->parent->group[i+1]),&(root->parent->group[i]));
                       root->parent->ptrs[i + 2] = root->parent->ptrs[i+1];
                       i--;
                }
                Copy_grp(&(root->parent->group[i+1]),&(Array[median_index]));
                root->parent->ptrs[i + 2] = newnode;
                newnode->parent = root->parent;
                root->parent->size++;
          }
     }
     return root;
}

grp* insert_grp(grp *root,int data_id,char data_name[],int data_wgg,Member array[]) {
    

    if (root == NULL) {
        root = create_grp();
        if (root == NULL) {
            
        } 
        else {
            root->group[0].grp_id = data_id;
            strcpy(root->group[0].grp_name,data_name);
            root->group[0].weekly_group_goal = data_wgg;
            
            for(int i=0;i<MAX_IND_GRP;i++)
            {
                root->group[0].member[i].mem_id=array[i].mem_id;
                root->group[0].member[i].mem_ptr=array[i].mem_ptr;
            }
            root->size = 1;
        }
    } 
    else {
        if(root->ptrs[0] == NULL)
        {
           if(root->size == ORDER - 1)
           {
           
               root = split_grp(root,data_id,data_name,data_wgg,array);
               
           }
           else
           {
                int i = root->size - 1;
                while (i >= 0 && root->group[i].grp_id > data_id) {
                    Copy_grp(&(root->group[i+1]),&(root->group[i]));
                    i--;
                }
                root->group[i+1].grp_id = data_id;
                strcpy(root->group[i+1].grp_name,data_name);
                root->group[i+1].weekly_group_goal = data_wgg;
            
                for(int j=0;j<MAX_IND_GRP;j++)
                {
                   root->group[i+1].member[j].mem_id=array[j].mem_id;
                   root->group[i+1].member[j].mem_ptr=array[j].mem_ptr;
                }
                root->size++;
           }
        }
        else
        {
            int i = root->size - 1;
            while (i >= 0 && root->group[i].grp_id > data_id) {
               i--;
            }
            root->ptrs[i + 1] = insert_grp((root->ptrs[i + 1]), data_id,data_name,data_wgg,array);
             
         }
     }
     
     return root;
}               

status_code extract_ind_csv() 
{
    FILE *file = fopen("try.csv", "r");
    if (file == NULL) 
    {
        printf("INVALID FILE");
        exit(EXIT_FAILURE);
    }

    status_code sc = SUCCESS;
    //individual *ptr=NULL;
    char line[MAX_LINE_LENGTH];
    ind* root;
    while (fgets(line, sizeof(line), file) != NULL) 
    {
        int data_id;
        char data_name[SIZE];
        int data_age;
        int data_grp_id;
        int data_dsg;
        int data_step_count[WEEK];
        sscanf(line, "%d,%99[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &data_id,
               data_name,
               &data_age,
               &data_grp_id,
               &data_dsg,
               &data_step_count[0],
               &data_step_count[1],
               &data_step_count[2],
               &data_step_count[3],
               &data_step_count[4],
               &data_step_count[5],
               &data_step_count[6]);
        root=insert(ind_root,data_id,data_name,data_age,data_grp_id,data_dsg,data_step_count);
        if(ind_root==NULL) ind_root =root;
        individual_id = data_id;
    }
    
    fclose(file);
    return sc;
}

ind* Search(ind *root,int *indptr, int key) {
    //printf("SEARCH\n");
    if (root == NULL)
    {
        //printf("NULL\n");
        return NULL;
    }
    // Find the smallest index i such that key <= node->keys[i]
    int i = 0;
    while (i < root->size && key > root->individual[i].ind_id)
        i++;

    // If the key is found at index i in this node
    if (i < root->size && key == root->individual[i].ind_id)
    {
        //printf("ptr = %p\tindex = %d\n",root,i);
        *indptr = i;
        return root;
    }
    // If key is greater than the largest key in this node, search the right subtree
    if (i == root->size)
        return Search(root->ptrs[i],indptr, key);

    // Otherwise, search the left subtree of the ith child
    return Search(root->ptrs[i],indptr, key);
}

status_code extract_grp_csv() 
{
    FILE *file = fopen("grp.csv", "r");
    if (file == NULL) 
    {
        printf("INVALID FILE");
        exit(EXIT_FAILURE);
    }

    status_code sc = SUCCESS;
    ind *ptr=NULL;
    char line[MAX_LINE_LENGTH];
    int temp_id[MAX_IND_GRP];
    char name[SIZE];
    grp *root;
    int grp_id,data_wgl;
    Member array[MAX_IND_GRP];

    while (fgets(line, sizeof(line), file) != NULL) 
    {
        

        sscanf(line, "%d,%99[^,],%d,%d,%d,%d,%d,%d\n",
               &grp_id,
               name,
               &array[0].mem_id,
               &array[1].mem_id,
               &array[2].mem_id,
               &array[3].mem_id,
               &array[4].mem_id,
               &data_wgl
               );
         //printf("%d\n",grp_id);
         for(int i=0;i< MAX_IND_GRP;i++)
         {
            int ind =-1;
            ptr=Search(ind_root,&ind,array[i].mem_id);
            //printf("mem id=%d\t",array[i].mem_id);
            if(ptr == NULL)
                   array[i].mem_ptr = NULL;
            else
             {      
                   array[i].mem_ptr = &(ptr->individual[ind]);
                   //printf("mem id:%d\n",ptr->individual[ind].ind_id);
              }
         }
         root = insert_grp(grp_root,grp_id,name,data_wgl,array);
         if(grp_root == NULL) grp_root = root;
         group_id = grp_id;
          
        
    }
    fclose(file);
    return sc;
}
void add_person()                    //It will add a new person using insert into a BTree
{
    //status_code sc=SUCCESS;
    char name[SIZE];
    int age,dsg;
    ind* root;
    getchar();
    printf("Enter the name: ");
    fgets(name,sizeof(name),stdin);
    name[strcspn(name, "\n")] = '\0';
    printf("Enter the age: ");
    scanf("%d",&age);
    printf("Enter the daily step goal: ");
    scanf("%d",&dsg);
    int step[WEEK];
    for(int i=0;i<WEEK;i++)
    {
          step[i]=0;
    }
    individual_id++;
    root = insert(ind_root,individual_id,name,age,0,dsg,step);
    if(ind_root == NULL) ind_root = root;

}       

grp* grpSearch(grp *root,int *grpptr, int grp_id);

void delete_individual(ind *root, int key) 
{            //Deleting individual 
    if (root == NULL) 
    {
        printf("The tree is empty!\n");
        return;
    }

    // Find the key to delete
    int i = 0;
    while (i < root->size && key > root->individual[i].ind_id) {
        i++;
    }

    // If key is found in this node
    if (i < root->size && key == root->individual[i].ind_id) 
    {
        
        // If the node is a leaf node
        if (root->ptrs[0] == NULL) {
            // Shift keys and pointers to fill the gap
            if(root->individual[i].group_id!=0)
            { 
                   printf("IF\n");
                   int ind = -1;
                   grp *search = grpSearch(grp_root,&ind,root->individual[i].group_id);
                   for (int k = 0; k < MAX_IND_GRP; k++) 
                   {
                     if (search->group[ind].member[k].mem_id == root->individual[i].ind_id) 
                     {
                           printf("FOUND\n");
                           search->group[ind].member[k].mem_id = 0;
                           search->group[ind].member[k].mem_ptr = NULL;

                            // Shift elements after index i one position to the left
                            for (int j = k; j < MAX_IND_GRP - 1; j++) 
                            {
                                         search->group[ind].member[j] = search->group[ind].member[j + 1];
                            }
                    
                            // Optionally, you might want to clear the last element after shifting
                             search->group[ind].member[MAX_IND_GRP - 1].mem_id = 0;
                             search->group[ind].member[MAX_IND_GRP - 1].mem_ptr = NULL;

                             break;
                     }
                   }
             }      
            for (int j = i; j < root->size - 1; j++) 
            {
                Copy(&(root->individual[j]),&(root->individual[j+1]));
            }
            root->size--;
            printf("Key %d deleted successfully!\n", key);
        } 
        else 
        { // If the node is an internal node
            // Find the predecessor (rightmost key in left subtree)
            ind *pred = root->ptrs[i];
            while (pred->ptrs[pred->size] != NULL) 
            {
                pred = pred->ptrs[pred->size];
            }
            // Replace the key with predecessor
            Copy(&(root->individual[i]),&(pred->individual[pred->size - 1]));
            // Delete the predecessor key from its node
            delete_individual(pred, (pred->individual[pred->size - 1].ind_id));
        }
    } 
    else 
    { // If key is not found in this node
        delete_individual(root->ptrs[i], key);
    }
}

status_code create_group()
{

    status_code sc=SUCCESS;
    char ans='y';
    char name[SIZE];
    int id,wgg;
    getchar();
    printf("ENTER GROUP NAME:");
    fgets(name,SIZE,stdin);
    name[strcspn(name,"\n")]='\0';
    printf("ENTER WEEKLY GROUP GOAL: ");
    scanf("%d",&wgg);
    int count=0;
    Member array[MAX_IND_GRP];
    for(int i=0;i<MAX_IND_GRP;i++)
    {
         array[i].mem_id = 0;
         array[i].mem_ptr = NULL;
    }
    grp* root;
    ind* temp;
    while(count<5 && ans=='y')
    {
       int ind = -1;
       printf("Enter id of member :\n");
       scanf("%d",&array[count].mem_id);
       getchar();
       temp=Search(ind_root,&ind,array[count].mem_id);
       //printf("ind_root->individual[0].ind_id = %d\n",ind_root->individual[0].ind_id);
       if(temp==NULL)
       {
          printf("THE ENTERED INDIVIDUAL ID IS INVALID!!\n");
       }
       else 
       {
          //printf("ELSE\n");
          if(temp->individual[ind].group_id!=0)
          {
             printf("THIS PARTICULAR INDIVIDUAL IS IN ANOTHER GROUP\n");
          }
          else
          {
             array[count].mem_ptr=&(temp->individual[ind]);
             count++;  
           }
        }
           printf("DO YOU WANT TO ADD MORE:(y/n)\n");
           scanf("%c",&ans);
      }
      if(count==5 && ans=='y')
      {
        printf("GROUP MEMBER LIMIT EXCEEDED!!\n");
      }
      if(count==0)
      {
         sc=FAILURE;
      }
      else
      {
          group_id++;
          root = insert_grp(grp_root,group_id,name,wgg,array);
          if(grp_root == NULL) grp_root = root; 
      }
      return sc;
}

void delete_group(grp *root, int key) {
    if (root == NULL) {
        printf("The group_id is invalid!\n");
        return;
    }

    // Find the key to delete
    int i = 0;
    while (i < root->size && key > root->group[i].grp_id) {
        i++;
    }

    // If key is found in this node
    if (i < root->size && key == root->group[i].grp_id) 
    {
        // If the node is a leaf node
        if (root->ptrs[0] == NULL) {
            // Shift keys and pointers to fill the gap
            for(int k=0;k<MAX_IND_GRP;k++)
            {
                   if(root->group[i].member[k].mem_ptr!=NULL)
                   {
                          root->group[i].member[k].mem_ptr->group_id = 0;
                   }
            }
            for (int j = i; j < root->size - 1; j++) {
                Copy_grp(&(root->group[j]),&(root->group[j+1]));
            }
            root->size--;
            printf("Key %d deleted successfully!\n", key);
        } 
        else 
        { // If the node is an internal node
            // Find the predecessor (rightmost key in left subtree)
            grp *pred = root->ptrs[i];
            while (pred->ptrs[pred->size] != NULL) {
                pred = pred->ptrs[pred->size];
            }
            // Replace the key with predecessor
            Copy_grp(&(root->group[i]),&(pred->group[pred->size -1]));
            // Delete the predecessor key from its node
            delete_group(pred, pred->group[pred->size - 1].grp_id);
        }
    } 
    else { // If key is not found in this node
        delete_group(root->ptrs[i], key);
    }
}

int calculate_total_steps(Individual ind) {
    int total_steps = 0, flag = 1;
    for (int i = 0; i < WEEK && flag == 1; i++) {
         
        if (ind.step_count[i] >= ind.daily_step_goal) {
            total_steps += ind.step_count[i];
        } 
        else 
        {
            flag = 0;
            total_steps = -1; // not in top_3
        }
    }
    
    return total_steps;
}

void update_top_3(Top_3* top3[], Individual* individual, int total_steps) 
{
    int flag = 1;
    for (int i = 0; i < 3 && flag == 1; i++) 
    {
        if (top3[i] == NULL || total_steps > top3[i]->steps) 
        {
            if (top3[i] == NULL) 
            {
                top3[i] = (Top_3*)malloc(sizeof(Top_3));
                if (top3[i] == NULL) {
                    fprintf(stderr, "Memory allocation failed\n");
                    return;
                }
            } 
            else 
            {
                for (int j = 2; j > i; j--) 
                { // Iterate from last to i
                    if (top3[j - 1] == NULL) 
                    { // Check if previous element is NULL
                        top3[j] = (Top_3*)malloc(sizeof(Top_3));
                    } 
                    else 
                    {
                        // If not NULL, allocate memory for new element and copy data
                        top3[j] = (Top_3*)malloc(sizeof(Top_3));
                        strcpy(top3[j]->name, top3[j - 1]->name);
                        top3[j]->ind_ID = top3[j - 1]->ind_ID;
                        top3[j]->steps = top3[j - 1]->steps;
                    }
                }
            }
            // Update the current position with new individual's data
            strcpy(top3[i]->name, individual->name);
            top3[i]->ind_ID = individual->ind_id;
            top3[i]->steps = total_steps;
            flag = 0;
        }
    }
}


void get_top3(ind* root, Top_3* ind_top3[]) {
    if (root == NULL)
        return;

    // Traverse the B-tree in inorder manner
    int i;
    for (i = 0; i < root->size; i++) {
        // Recursively traverse left subtree
        get_top3(root->ptrs[i], ind_top3);

        // Process the individual
        // Assuming you have a function to calculate total steps for an individual
        //printf("ID : %d",root->individual[i].ind_id);
        int total_steps = calculate_total_steps(root->individual[i]);
        //printf("Total steps: %d\n",total_steps);
        // If the current individual qualifies for top 3
        if (total_steps != -1) {
            update_top_3(ind_top3, &(root->individual[i]), total_steps);
        }
    }

    // Traverse the right subtree
    get_top3(root->ptrs[i], ind_top3);
}

void print_top3(Top_3* ind_top3[]) {
    int flag=1;
    printf("ID\tNAME\tTOTAL STEPS\n");
    for (int i = 0; i < 3&&flag==1; i++) {
        if (ind_top3[i] != NULL) {
            printf("%d\t%s\t%d\n", ind_top3[i]->ind_ID, ind_top3[i]->name, ind_top3[i]->steps);
        } 
        else 
        {
            flag=0;
        }
    }
}

grp* grpSearch(grp *root,int *grpptr, int grp_id) 
{
    if (root == NULL)
    {
        return NULL;
    }
    // Find the smallest index i such that key <= node->keys[i]
    int i = 0;
    while (i < root->size && grp_id > root->group[i].grp_id)
    {
        i++;
    }
    
    // If the key is found at index i in this node
    if (i < root->size && grp_id == root->group[i].grp_id)
    {
        *grpptr = i;
        return root;
    }
    // If key is greater than the largest key in this node, search the right subtree
    if (i == root->size)
    {  
        return grpSearch(root->ptrs[i],grpptr, grp_id);
    }
    // Otherwise, search the left subtree of the ith child
    return grpSearch(root->ptrs[i],grpptr, grp_id);
}


void Check_group_achievement(int grp_id)
{
   int i=0;
   grp*root=grp_root;
   grp*grp1=grpSearch(root,&i,grp_id);
   if(grp1==NULL)
   {
      printf("Group ID does not exist!!");
   }
   else
   {
       // Calculating total steps taken by the group
        int total_steps = 0;
        for (int mem_index = 0; mem_index < MAX_IND_GRP; mem_index++)
        {
            if (grp1->group[i].member[mem_index].mem_ptr != NULL)
            {
                Individual *ind_ptr = grp1->group[i].member[mem_index].mem_ptr;
                for (int week = 0; week < WEEK; week++)
                {
                    total_steps += ind_ptr->step_count[week];
                }
            }
        }

        // Checking if the total steps taken by the group
        // exceeds the weekly group goal
        if (total_steps >= grp1->group[i].weekly_group_goal)
        {
            printf("Group has achieved its weekly group goal!\n");
        }
        else
        {
            printf("Group has not achieved its weekly group goal!\n");
        }
    }
}
         
void Check_individual_rewards(int id)
{
    ind*root=ind_root;
    int ptr,flag=1,flag1=1;
    ind*ind1=Search(root,&ptr,id);
    if(ind1==NULL)
    {
       printf("INVALID ID!!");
    }
    else
    {
       Top_3* top3[3] = {NULL};
       get_top3(root, top3);
       printf("ID\tNAME\t\tSCORE\n");
        for (int i = 0; i < 3&&flag==1&&flag1==1; i++) 
        {
            if (top3[i] != NULL) 
            { 
               if(top3[i]->ind_ID==id)
               { 
                   if(i==0)
                   {
                       printf("%d\t%s\t100\n", top3[i]->ind_ID,top3[i]->name);
                   }
                   if(i==1)
                   {
                       printf("%d\t%s\t75\n", top3[i]->ind_ID, top3[i]->name);   
                   }
                   if(i==2)
                   {
                       printf("%d\t%s\t50\n", top3[i]->ind_ID, top3[i]->name);    
                   }
                   flag1=0;
                }
             }
             else
             {
                flag=0;
             }
         }
         if(flag1==1)
         {
            printf("%d\t%s\t0\n", ind1->individual[ptr].ind_id, ind1->individual[ptr].name);          
         }
    }
}                

void Suggest_goal_update(int id) {
    ind* root = ind_root;
    int ptr, flag = 1, total_steps = 0, flag1 = 1;
    ind* ind1 = Search(root, &ptr, id);

    if (ind1 == NULL) 
    {
        printf("INVALID ID!!");
    }
    else 
    {
        Top_3* top3[3] = {NULL};
        get_top3(root, top3);
        for (int i = 0; i < 3 && flag == 1 && flag1 == 1; i++) 
        {
            if (top3[i] != NULL) 
            {
                total_steps += top3[i]->steps;
                if (top3[i]->ind_ID == id) 
                {
                    printf("Already in top 3!!");
                    flag1 = 0;
                }
            } 
            else 
            {
                flag = 0;
            }
        }
        
        if (flag1 == 1) 
        {
            int goal_update = total_steps / 21;
            char ans;
            printf("Goal update suggestion: %d\n", goal_update);
            printf("Want to update goal [y/n]: ");
            // Clear input buffer before reading character
            while (getchar() != '\n'); // Clear input buffer
            scanf("%c", &ans);
            if (ans == 'y') 
            {
                ind1->individual[ptr].daily_step_goal = goal_update;
                printf("GOAL UPDATED...KEEP WALKING\n");
            }
        }
    }
}

void Merge_groups(int grp_id1, int grp_id2)
{
    int ptr1, ptr2;
    grp* root = grp_root;
    grp* grp1 = grpSearch(root, &ptr1, grp_id1);
    grp* grp2 = grpSearch(root, &ptr2, grp_id2);
    
    if (grp1 == NULL || grp2 == NULL)
    {
        printf("INVALID ID!!");
    }
    else
    {
        int grp1_mem = 0, grp2_mem = 0;
        
        // Counting members in grp1
        while (grp1->group[ptr1].member[grp1_mem].mem_ptr != NULL && grp1_mem < MAX_IND_GRP)
        {
            grp1_mem++;
        }
        
        // Counting members in grp2
        while (grp2->group[ptr2].member[grp2_mem].mem_ptr != NULL && grp2_mem < MAX_IND_GRP)
        {
            grp2_mem++;
        }
        
        // Checking if the merged group will exceed the limit
        if (grp1_mem + grp2_mem > MAX_IND_GRP)
        {
            printf("GROUPS CAN'T BE MERGED EXCEEDING GROUP LIMIT");
        }
        else
        {
            char name[SIZE];
            int wsg;
            int size=0;
            getchar();
            printf("ENTER NEW GROUP NAME:\n");
            fgets(name, SIZE, stdin);
            name[strcspn(name, "\n")] = '\0';
            
            printf("ENTER WEEKLY GROUP GOAL: \n");
            scanf("%d", &wsg);
            
            Member array[MAX_IND_GRP];
            printf("total members in group1: %d",grp1_mem);
            // Copy members of grp1 to array
            for (int i = 0; i < grp1_mem; i++)
            {
                array[i].mem_id = grp1->group[ptr1].member[i].mem_id;
                array[i].mem_ptr = grp1->group[ptr1].member[i].mem_ptr;
                size++;
            }
            
            // Copy members of grp2 to array
            for (int i = 0; i < grp2_mem; i++)
            {
                array[grp1_mem + i].mem_id = grp2->group[ptr2].member[i].mem_id;
                array[grp1_mem + i].mem_ptr = grp2->group[ptr2].member[i].mem_ptr;
                size++;
            }
            
            for(int i=size;i<MAX_IND_GRP;i++)
            {
                 array[i].mem_id = 0;
                 array[i].mem_ptr = NULL;
            }
            // Deleting the old groups
            delete_group(root, grp_id1);
            delete_group(root, grp_id2);
            
            // Inserting the new merged group
            grp* new_grp = insert_grp(grp_root, ++group_id, name, wsg, array);
            
            if (grp_root == NULL)
                grp_root = new_grp;
            
            int newptr;
            grp* merged_group = grpSearch(root, &newptr, group_id);
            
            // Updating group_id for each member of the new merged group
            for (int i = 0; i < grp1_mem + grp2_mem; i++)
            {
               printf("MEMBER ID: %d\n",array[i].mem_ptr->ind_id);
               array[i].mem_ptr->group_id = merged_group->group[newptr].grp_id;
            }
        }
    }
}




void print_individuals_in_b_tree(ind *root) 
{
    if (root != NULL) 
    {
        //printf("root = %p\n && root->size = %d\n",root,root->size);
        int i;
        for (i = 0; i < root->size; i++) {
            //printf("i=%d\n",i);
            print_individuals_in_b_tree(root->ptrs[i]);
            printf("ID =%d\n", root->individual[i].ind_id);
            printf("Name : %s\n",root->individual[i].name);
            printf("age =%d\n", root->individual[i].age);
            printf("group_id=%d\n",root->individual[i].group_id);
            printf("daily_goal=%d\n",root->individual[i].daily_step_goal);
            printf("Steps:");
            for(int j=0;j<WEEK;j++)
                printf("%d\t",root->individual[i].step_count[j]);
            //printf("Total steps:%d",calculate_total_steps(root->individual[i]));
            printf("\n\n");
        }
        print_individuals_in_b_tree(root->ptrs[i]);
    }
}

void print_group_range(grp *root,int grp_id1,int grp_id2) 
{
    if (root != NULL) 
    {
        int i;
        for (i = 0; i < root->size; i++) 
        {
           print_group_range(root->ptrs[i],grp_id1,grp_id2);
            if(root->group[i].grp_id>=grp_id1 && root->group[i].grp_id<=grp_id2)
            {
               printf("ID = %d\n", root->group[i].grp_id);
               printf("Name: %s\n", root->group[i].grp_name);
               printf("Daily Goal: %d\n", root->group[i].weekly_group_goal);
               printf("Members:\n");
               for (int j = 0; j < MAX_IND_GRP && root->group[i].member[j].mem_ptr != NULL; j++) 
               {
                   printf("  - Individual ID: %d\n", root->group[i].member[j].mem_ptr->ind_id);
               }
               printf("\n\n");
           }
        }   
        print_group_range(root->ptrs[i],grp_id1,grp_id2);
    }
}

/*void Display_group_range_info(int grp_id1,int grp_id2)
{
    int ptr1, ptr2;
    grp* root = grp_root;
    grp* grp1 = grpSearch(root, &ptr1, grp_id1);
    grp* grp2 = grpSearch(root, &ptr2, grp_id2);
    
    if (grp1 == NULL || grp2 == NULL)
    {
        printf("INVALID ID!!");
    }
    else
    {
        //grp*root=grp_root;
        print_group_range(root,grp_id1,grp_id2);
        
     }   
}*/     
       

void print_group_in_b_tree(grp *root) {
    if (root != NULL) {
        int i;
        for (i = 0; i < root->size; i++) {
            print_group_in_b_tree(root->ptrs[i]);
            printf("ID = %d\n", root->group[i].grp_id);
            printf("Name: %s\n", root->group[i].grp_name);
            printf("Weekly Goal: %d\n", root->group[i].weekly_group_goal);
            printf("Members:\n");
            for (int j = 0; j < MAX_IND_GRP && root->group[i].member[j].mem_ptr != NULL; j++) {
                printf("  - Individual ID: %d\n", root->group[i].member[j].mem_ptr->ind_id);
            }
            printf("\n\n");
        }
        print_group_in_b_tree(root->ptrs[i]);
    }
}

cpygrp* create_cpygrp() 
{
    cpygrp* new_node = (cpygrp*)malloc(sizeof(cpygrp));
    if (new_node != NULL) {
        new_node->size = 0;
        for(int i = 0; i < ORDER; i++) {
            new_node->ptrs[i] = NULL;
        }
    }
    new_node->parent = NULL;
    return new_node;
}
cpygrp* cpy_grp_root = NULL;
void Copy_cpygrp(Copy_Group* ptr1,Copy_Group* ptr2)
{
    //printf("%d\n",ptr2->ind_id);
    ptr1->grp_id = ptr2->grp_id;
    strcpy(ptr1->grp_name,ptr2->grp_name);
    ptr1->total_steps=ptr2->total_steps;
}

cpygrp* split_cpygrp(cpygrp* root,int data_id,char data_name[],int steps)
{
    //printf("SPLIT\n");
     int i = 0;
    Copy_Group Array[ORDER];
    while(i < (ORDER - 1) && root->group[i].total_steps > steps)
    {
          //printf("While 1\n");
          Copy_cpygrp(&(Array[i]),&(root->group[i]));
          i++;
    }
    //printf("While 1 completed\n");
    Array[i].grp_id = data_id;
    strcpy(Array[i].grp_name,data_name);
    Array[i].total_steps = steps;
    i++;
    while(i < (ORDER))
    {
          Copy_cpygrp(&(Array[i]),&(root->group[i-1]));
          i++;
    }
    int median_index = (ORDER-1)/2;
    //printf("median_index = %d\n",median_index);
     if(root->parent == NULL)
     {
          cpygrp* newnode = create_cpygrp();
          cpygrp* parent = create_cpygrp();
          
          Copy_cpygrp(&(parent->group[0]),&(Array[median_index]));
          parent->size = 1;
          parent->ptrs[0]=root;
          for(i=0;i<median_index;i++)
          {
                Copy_cpygrp(&(root->group[i]),&(Array[i]));
          }
          root->size = median_index;
          int k = 0;
          for(i=median_index + 1;i< ORDER;i++)
          {
                Copy_cpygrp(&(newnode->group[k]),&(Array[i]));
                k++;
          }
          newnode->size = k;
          parent->ptrs[1]=newnode;
          i = ORDER - 1;
          k=newnode->size;
          while(i > median_index + 2)
          {
               newnode->ptrs[k] = root->ptrs[i];
               k--;
               if(root->ptrs[i]!= NULL)
                   root->ptrs[i]->parent = newnode;
               i--;
          }
          if( root->ptrs[median_index + 2] != NULL)
          {
               cpygrp *extra = create_cpygrp();
               root->ptrs[median_index + 2]->size = median_index;
               root->ptrs[median_index + 2]->parent = newnode;
               newnode->ptrs[k] = extra;
               newnode->ptrs[k - 1] = root->ptrs[median_index + 2];
               int k = 0;
               for(i=median_index ;i< ORDER - 1;i++)
               {
                   Copy_cpygrp(&(extra->group[k]),&(root->ptrs[median_index + 2]->group[i]));
                   k++;
               }
               extra->size = k;
               extra->parent = newnode;
               newnode->ptrs[0] = root->ptrs[median_index + 1];
               if(root->ptrs[median_index]!=NULL)
               root->ptrs[median_index + 1]->parent = newnode;
          }
          
          
          
          root->parent=parent;
          newnode->parent=parent;
          cpy_grp_root=parent;
          return parent;
      }
      
      else
      {
          if(root->parent->size == ORDER - 1 )
          {
                for(i=0;i<median_index;i++)
                { 
                     Copy_cpygrp(&(root->group[i]),&(Array[i]));
                }
                for(i=median_index+1;i<ORDER;i++)
                {
                    Copy_cpygrp(&(root->group[i-1]),&(Array[i]));
                }
                root = split_cpygrp(root->parent, Array[median_index].grp_id, Array[median_index].grp_name,
                Array[median_index].total_steps);
                
          }
          else
          {
                //i = root->parent->size - 1;
                cpygrp* newnode = create_cpygrp();
                if(newnode == NULL)
                {
                     printf("MEMORY ALLOCATION FAILED\n");
                     return root;
                }
                for(i=0;i<median_index;i++)
                {
                     Copy_cpygrp(&(root->group[i]),&(Array[i]));
                }
                root->size = median_index;
                int k = 0;
                for(i=median_index+1;i<ORDER;i++)
                {
                     Copy_cpygrp(&(newnode->group[k]),&(Array[i]));
                     k++;
                }
                newnode->size = k;
                // If the parent node is not full, insert the value into this node
                i = root->parent->size - 1;
                while (i >= 0 && root->parent->group[i].total_steps > Array[median_index].total_steps) {
                       Copy_cpygrp(&(root->parent->group[i+1]),&(root->parent->group[i]));
                       root->parent->ptrs[i + 2] = root->parent->ptrs[i+1];
                       i--;
                }
                Copy_cpygrp(&(root->parent->group[i+1]),&(Array[median_index]));
                root->parent->ptrs[i + 2] = newnode;
                newnode->parent = root->parent;
                root->parent->size++;
          }
     }
     return root;
}

cpygrp* insert_cpygrp(cpygrp *root,int data_id,char data_name[],int steps) {
    

    if (root == NULL) {
        root = create_cpygrp();
        if (root == NULL) {
            
        } 
        else {
            root->group[0].grp_id = data_id;
            strcpy(root->group[0].grp_name,data_name);
            root->group[0].total_steps = steps;
            root->size = 1;
        }
    } 
    else {
        if(root->ptrs[0] == NULL)
        {
           if(root->size == ORDER - 1)
           {
           
               root = split_cpygrp(root,data_id,data_name,steps);
               
           }
           else
           {
                int i = root->size - 1;
                while (i >= 0 && root->group[i].total_steps < steps) {
                    Copy_cpygrp(&(root->group[i+1]),&(root->group[i]));
                    i--;
                }
                root->group[i+1].grp_id = data_id;
                strcpy(root->group[i+1].grp_name,data_name);
                root->group[i+1].total_steps = steps;
                root->size++;
           }
        }
        else
        {
            int i = root->size - 1;
            while (i >= 0 && root->group[i].total_steps < steps) {
               i--;
            }
            root->ptrs[i + 1] = insert_cpygrp((root->ptrs[i + 1]), data_id,data_name,steps);
             
         }
     }
     
     return root;
}               

int calculate_total_steps_grp(Group grp1)
{
     int sum = 0;
            for(int j=0;j<MAX_IND_GRP;j++)
            {
                 if(grp1.member[j].mem_ptr!=NULL)
                 {
                      for(int k=0;k<WEEK;k++)
                         sum= sum+(grp1.member[j].mem_ptr->step_count[k]);
                 }
            }  
      return sum;
}

void create_cpy_tree(grp *root) {
    if (root != NULL) {
        int i;
        cpygrp *root1 = NULL;
        for (i = 0; i < root->size; i++) {
            create_cpy_tree(root->ptrs[i]);
            
            printf("Total Steps:%d\n",calculate_total_steps_grp(root->group[i]));
            root1 = insert_cpygrp(cpy_grp_root,root->group[i].grp_id,root->group[i].grp_name, calculate_total_steps_grp(root->group[i]));
            if(cpy_grp_root==NULL) cpy_grp_root = root1;
            }
        create_cpy_tree(root->ptrs[i]);
    }
   }

void print_leader_board(cpygrp *root) {
    if (root != NULL) {
        int i;
        for (i = 0; i < root->size; i++) {
            print_leader_board(root->ptrs[i]);
            printf("ID = %d\t", root->group[i].grp_id);
            printf("Name: %s\t", root->group[i].grp_name);
            printf("Total Steps: %d\n", root->group[i].total_steps);
        }
        print_leader_board(root->ptrs[i]);
    }
}

void DeleteTree(cpygrp* root)
{
     if(root!=NULL)
     {
         int i;
         for(i=0;i<root->size;i++)
         {
              DeleteTree(root->ptrs[i]);
              //free(root);
         }
         free(root);
     }
}

status_code generate_leader_board() 
{
    // Make a copy of the  group list
    status_code sc=SUCCESS;
    
    create_cpy_tree(grp_root);
    

    // Display the leader board
    printf("Leader Board:\n");
    printf("Group ID\tGroup Name\t\tTotal Steps\n");

    print_leader_board(cpy_grp_root);

    // Free the memory allocated for the copied list
    DeleteTree(cpy_grp_root);

    return sc;
}

void print_ind_into_file(FILE* file,ind* root)
{
    if(root!=NULL)
    {
         int i;
         for(i=0;i<root->size;i++)
         {
         
             print_ind_into_file(file,root->ptrs[i]);
             fprintf(file, "%d,%s,%d,%d,%d,", root->individual[i].ind_id, root->individual[i].name, root->individual[i].age,root->individual[i].group_id,root->individual[i].daily_step_goal);
             for (int j = 0; j < WEEK; j++) 
             {
                fprintf(file, "%d", root->individual[i].step_count[j]);
                if (j < 6) fprintf(file, ",");
             }
             fprintf(file, "\n");
         }
         print_ind_into_file(file,root->ptrs[i]);
     }
}

void insert_ind_file()
{
   FILE *file = fopen("try.csv", "w");
   if (file == NULL) 
   {
        perror("Error opening file");
        exit(1);
   }
   print_ind_into_file(file,ind_root);
}   

void print_grp_into_file(FILE* file,grp* root)
{
    if(root!=NULL)
    {
         int i;
         for(i=0;i<root->size;i++)
         {
         
             print_grp_into_file(file,root->ptrs[i]);
             fprintf(file, "%d,%s,", root->group[i].grp_id, root->group[i].grp_name);
             for (int j = 0; j < MAX_IND_GRP; j++) 
             {
                fprintf(file, "%d", root->group[i].member[j].mem_id);
                 fprintf(file, ",");
             }
             fprintf(file,"%d",root->group[i].weekly_group_goal);
             fprintf(file, "\n");
         }
         print_grp_into_file(file,root->ptrs[i]);
     }
}

void insert_grp_file()
{
   FILE *file = fopen("grp.csv", "w");
   if (file == NULL) 
   {
        perror("Error opening file");
        exit(1);
   }
   print_grp_into_file(file,grp_root);
}

int main() 
{

   status_code sc;
   sc=extract_ind_csv();
   sc=extract_grp_csv();
   printf("\nWELCOME TO THE STEP SYNC!!\n");
   ind*root=ind_root;
   grp*grproot=grp_root;
   //print_individuals_in_b_tree(root);
   //print_group_in_b_tree(grproot);
   char ans='y';
   int n,id1=0,id2=0;
   while(ans=='y')
   {
      printf("\nAvailable features: \n");
      printf("1)Add Person\n2)Create Group\n3)Top 3 in individuals\n4)Group Achievement\n5)Leader Board\n6)Individual Reward\n7)Delete Individual\n8)Delete Group\n9)Merge Groups\n10)Display  Group Range Information\n11)Suggest goal update\n");
      printf("\nEnter the option number: ");
      scanf("%d",&n);
      switch(n) 
      {
        case 1:
            printf("Add Person\n");
            add_person();
             printf("Added Successfully!\n");
            
            break;
            
        case 2:
            printf("Create Group\n");
            sc=create_group();
            if(sc==SUCCESS) printf("Created Successfully!\n");
            else printf("Not created\n");
            break;
        
        case 3:
            printf("Top 3 in individuals\n");
            Top_3* top3[3] = {NULL};
            // Assuming you have a B-tree root called `ind_root`
            get_top3(root, top3);
            print_top3(top3);
            
            break;
        case 4:
            printf("Group Achievement\n");
            printf("Enter the Group ID number to check group achievement: ");
            scanf("%d",&id1);
            Check_group_achievement(id1);
            break;
        case 5:
            printf("Leader Board\n");
            sc=generate_leader_board();
            break;
        case 6:
            printf("Individual Reward\n");
            printf("Enter the Individual ID to find the reward: ");
            scanf("%d",&id1);
            Check_individual_rewards(id1);
            break;
        case 7:
            printf("Delete Individual\n");
            printf("Enter the Individual ID to delete the individual: ");
            scanf("%d",&id1);
            delete_individual(ind_root,id1);
             printf("Deleted Successfully!\n");
            
            break;
        case 8:
            printf("Delete Group\n");
            printf("Enter the Group ID to delete the group: ");
            scanf("%d",&id1);
            delete_group(grp_root,id1);
            
            break;
        case 9:
            printf("Merge Groups\n");
            printf("Enter the Group ID 1 to merge the groups: ");
            scanf("%d",&id1);
            printf("Enter the Group ID 2 to merge the groups: ");
            scanf("%d",&id2);
            Merge_groups(id1,id2);
            grproot=grp_root;
            root=ind_root;
            print_group_in_b_tree(grproot);
            //print_individuals_in_b_tree(root);
            break;
        case 10:
            printf("Display  Group Range Information\n");
            printf("Enter the Group ID to display the information: ");
            scanf("%d",&id1);
            printf("Enter the Group ID to display the information: ");
            scanf("%d",&id2);
            grproot=grp_root;
            print_group_range(grproot,id1, id2);
            break;
        case 11:
            printf("Suggest goal update\n");
            printf("Enter the individual ID for suggestion in goal updation: ");
            scanf("%d",&id1);
           Suggest_goal_update(id1);
            break;
        default:
            printf("Invalid option\n");
            // Handle invalid option
            break;
        
    }
    getchar();
    printf("\nWant to perform more operations?[y/n]\n");
    scanf("%c",&ans);    
  }  
  printf("\nThank You!!!....Keep walking:)\n");
  insert_ind_file();
  insert_grp_file();
  /*DeleteAllIndividual();
  DeleteAllGroup();*/
  /*printf("%ld\n",sizeof(individual));
  printf("%ld\n",sizeof(group));
  printf("%ld\n",sizeof(member));*/
  return 0;
}
