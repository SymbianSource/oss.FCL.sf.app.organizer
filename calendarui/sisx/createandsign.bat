@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem

@echo -------- Create sis packages ---------
makesis calendarui_armv5_urel.pkg

@echo -------- Sign and rename sis packages -----------
signsis calendarui_armv5_urel.sis hb_calendarui.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
	
