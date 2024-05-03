#include "DisplayHandler.hpp" 

class _MainUi : public DisplayHandler{

    protected:
      virtual void onDraw(U8G2 &disp);
};

extern _MainUi MainUi;