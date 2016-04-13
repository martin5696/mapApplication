#include "kdTreeNode.h"

bool operator == (const coordinates& lhs,const coordinates& rhs){
        if((lhs.xy[0] == rhs.xy[0] && lhs.xy[1] == rhs.xy[1]) || lhs.ID == rhs.ID)
            return true;
        else 
            return false;
}
