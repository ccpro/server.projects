#ifndef _CPP_PALM_CATEGORY_H_
#define _CPP_PALM_CATEGORY_H_

#include "common.h"

class Category_DAT
{
   public:
      ulong	index;
      ulong	id;
      ulong	dirty_flag;
      string	long_name;
      string	short_name;
   
      Category_DAT(){init();};
   
      void init();
};
          
class CPalmCategory
{
   private:
      vector<Category_DAT*> palm_category;
      int count;
              
   public:
      CPalmCategory(int count=0);
      ~CPalmCategory();
                  
      int  getCount(){return palm_category.size();};
                    
      int read(ifstream &ifs);
      int print();
};

#endif // _CPP_PALM_CATEGORY_H_
