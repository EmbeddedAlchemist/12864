#line 1 "C:\\Users\\CHALLENGER\\Documents\\KeXieData\\Project\\12864\\src\\Display\\MainUi.hpp"
#include "DisplayHandler.hpp" 

class _MainUi : public DisplayHandler{

    protected:
      virtual void onDraw(U8G2 &disp);
};

extern _MainUi MainUi;