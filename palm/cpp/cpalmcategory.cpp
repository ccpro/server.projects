#include "cpalmcategory.hpp"

void Category_DAT::init()
{
   index = id = dirty_flag = 0;
}

CPalmCategory::CPalmCategory(int cnt)
{
cout << "CPalmCategory start" << endl;
   if (cnt > 0) {
      count = cnt;
   } else {
      count = 0;
   }
}
            
CPalmCategory::~CPalmCategory()
{
   if ( palm_category.size() > 0 ) {
      palm_category.clear();
   }
   cout << "CPalmCategory done" << endl;
}

int CPalmCategory::read(ifstream &ifs)
{
  int i;
  Category_DAT *t;
      
   if ( !ifs.is_open() ) {
      cout << "CPalmCategory:: wasn't open" << endl;
      return 1;
   }         

   if ( palm_category.size() > 0 ) {
      palm_category.clear();
   }

cout << "count for read:" << count << endl;
                  
   for(i=0; i < count; i++) {
      t = new Category_DAT;
      palm_category.push_back(t);
                            
      ifs.read((char*)(void*)&t->index, sizeof(t->index));
      ifs.read((char*)(void*)&t->id, sizeof(t->id));
      ifs.read((char*)(void*)&t->dirty_flag, sizeof(t->dirty_flag));
      t->long_name = read_string(ifs);
      t->short_name = read_string(ifs);
   }
   
   cout << "-- CPalmCategory read " << i << endl;
   return 0;
}

int CPalmCategory::print()
{
  vector<Category_DAT*>::iterator iter;
  Category_DAT *t;

   cout << "category list:" << endl;      
   if ( !palm_category.size() ) {
      cout << "no data was read" << endl;
      return 0;
   }         

   cout << "count:" << palm_category.size() << endl;
   for(iter = palm_category.begin(); iter != palm_category.end(); iter++) {
      t = *iter;

      cout << "index:" << t->index << ", id:" << t->id << ", dflag:" << t->dirty_flag << endl;
      cout << t->short_name << " :: " << t->long_name << endl;
                            
   }
   return 0;
}
