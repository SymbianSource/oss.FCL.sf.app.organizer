/*
 * calenservices.h
 *
 *  Created on: Jun 21, 2010
 *      Author: zabinpio
 */

#ifndef CALENSERVICES_H_
#define CALENSERVICES_H_

#include "calencontext.h"

/*!
 Mocked class MCalenServices
 */
class AgendaUtil;

class MCalenServices
{
public:
    MCalenServices() {
        
    }
    
    MCalenContext& Context(){
			return mContext;
    	}
    
    int getFirstView(){ return 0;}
    TBool IssueCommandL( TInt aCommand  ){ Q_UNUSED(aCommand); return EFalse;}
    AgendaUtil* agendaInterface() {return 0;}
    
    ~MCalenServices() {
        
    }
    
    MCalenContext mContext;
};
#endif /* CALENSERVICES_H_ */
