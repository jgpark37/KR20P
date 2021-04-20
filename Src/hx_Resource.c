/*
*******************************************************************************
*
*                              Hexar Systems, Inc.
*                      104, Factory Experiment Bldg, No41.55
*              Hanyangdaehak-ro, Sangnok-gu, Ansan, Gyeonggi-do, Korea
*
*                (c) Copyright 2017, Hexar Systems, Inc., Sangnok-gu, Ansan
*
* All rights reserved. Hexar Systemsâ€™s source code is an unpublished work and the
* use of a copyright notice does not imply otherwise. This source code contains
* confidential, trade secret material of Hexar Systems, Inc. Any attempt or participation
* in deciphering, decoding, reverse engineering or in any way altering the 
source
* code is strictly prohibited, unless the prior written consent of Hexar Systems, Inc.
* is obtained.
*
* Filename		: hx_Resource.c
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2019/08/31
* Description		: resource data
*******************************************************************************
*
*/

/* Includes ------------------------------------------------------------------*/
#include "hx_Resource.h"

/* Private define ------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
char *pBtnInfo[] = {
	//up						down
	//"i ldsu.bmp,380,211\r", "i ldsd.bmp,380,211\r",
	//"i out.bmp,345,0\r",		"i out.bmp,345,0\r",			//»ç¿ë ¾ÈÇÔ
	"i usca.bmp,184,213\r",		"i uscap.bmp,184,213\r",
	"i uscak.bmp,184,213\r",	"i uscapk.bmp,184,213\r",
	"i uscac.bmp,184,213\r",	"i uscapc.bmp,184,213\r",
	//90%
	"i pirsok.bmp,184,150\r",  	"i ppirsok.bmp,184,150\r",
	"i pirsokk.bmp,184,150\r",  "i ppirsokk.bmp,184,150\r",
	"i pirsok.bmp,184,150\r",  	"i ppirsok.bmp,184,150\r",
	//full
	"i fogye.bmp,182,150\r",	"i pfogye.bmp,182,150\r",
	"i fogyek.bmp,182,150\r",	"i pfogyek.bmp,182,150\r",
	"i fogye.bmp,182,150\r",	"i pfogye.bmp,182,150\r",
	"i fogno.bmp,245,150\r",	"i pfogno.bmp,245,150\r",
	"i fognok.bmp,245,150\r",	"i pfognok.bmp,245,150\r",
	"i fogno.bmp,245,150\r",	"i pfogno.bmp,245,150\r",
	//fullcheck
	"i fogye.bmp,122,150\r",	"i pfogye.bmp,122,150\r",
	"i fogyek.bmp,122,150\r",	"i pfogyek.bmp,122,150\r",
	"i fogye.bmp,122,150\r",	"i pfogye.bmp,122,150\r",
	"i fogno.bmp,245,150\r",	"i pfogno.bmp,245,150\r",
	"i fognok.bmp,245,150\r",	"i pfognok.bmp,245,150\r",
	"i fogno.bmp,245,150\r",	"i pfogno.bmp,245,150\r",
	//Run
	//"i chus.bmp,4,55\r",		"i chusp.bmp,4,54\r",			//»ç¿ë ¾ÈÇÔ
	//"i usse.bmp,4,140\r",		"i ussep.bmp,4,139\r",			//»ç¿ë ¾ÈÇÔ
	//"i syse.bmp,4,221\r",		"i sysep.bmp,4,220\r",			//»ç¿ë ¾ÈÇÔ
	//"i play.bmp,176,44\r",		"i playp.bmp,176,44\r",			//»ç¿ë ¾ÈÇÔ
	//"i home.bmp,271,186\r",	"i homep.bmp,271,186\r",		//»ç¿ë ¾ÈÇÔ
	//pwdforger
	"i fogye.bmp,122,150\r",	"i pfogye.bmp,122,150\r",
	"i fogno.bmp,245,150\r",	"i pfogno.bmp,245,150\r",
	//login
	"i num1.bmp,255,48\r",		"i ppinum.bmp,255,48\r",
	"i num2.bmp,303,48\r",		"i ppinum.bmp,303,48\r",
	"i num3.bmp,351,48\r",		"i ppinum.bmp,351,48\r",
	"i num4.bmp,399,48\r",		"i ppinum.bmp,399,48\r",
	"i num5.bmp,255,103\r",	"i ppinum.bmp,255,103\r",
	"i num6.bmp,303,103\r",	"i ppinum.bmp,303,103\r",
	"i num7.bmp,351,103\r",	"i ppinum.bmp,351,103\r",
	"i num8.bmp,399,103\r",	"i ppinum.bmp,399,103\r",
	"i num9.bmp,255,158\r",	"i ppinum.bmp,255,158\r",
	"i num0.bmp,303,158\r",	"i ppinum.bmp,303,158\r",
	"i pwdbc.bmp,352,158\r",	"i ppwdbc.bmp,352,158\r",
	"i pwdok.bmp,184,222\r",	"i ppwdok.bmp,184,222\r",
	"i pwdcan.bmp,320,222\r",	"i ppwdcan.bmp,320,222\r",
	//Product
	"i cali.bmp,22,54\r",		"i calip.bmp,22,54\r",
	"i pccn.bmp,22,117\r",		"i ppccn.bmp,22,117\r",
	"i prbc.bmp,389,4\r",		"i pprbc.bmp,389,4\r",
	"i pdbt.bmp,22,105\r",		"i ppdbt.bmp,22,105\r",	
	"i pdbt.bmp,22,160\r",		"i ppdbt.bmp,22,160\r",	
	//Calibration
	"i cali.bmp,49,40\r",		"i calip.bmp,49,40\r",			//Calibration 
	"i cali.bmp,49,85\r",		"i calip.bmp,49,85\r",
	"i cali.bmp,49,131\r",		"i calip.bmp,49,131\r",
	"i cali.bmp,49,177\r",		"i calip.bmp,49,177\r",
	"i cali.bmp,49,224\r",		"i calip.bmp,49,224\r",
	"i ret.bmp,362,46\r",		"i pret.bmp,362,46\r",
	"i prbc.bmp,389,4\r",		"i pprbc.bmp,389,4\r",			//³ª°¡±â 
	//Home
	"i measureu.bmp,85,84\r",	"i measured.bmp,85,84\r",			//measure
	"i rehabu.bmp,285,84\r",	"i rehabd.bmp,285,84\r",			//rehab
	//User
	"i guestu.bmp,85,84\r",		"i guestd.bmp,85,84\r",			//Geust
	"i useru.bmp,285,84\r",		"i userd.bmp,285,84\r",			//User
	//Angle measure
	"i left.bmp,44,222\r",		"i leftp.bmp,44,222\r",
	"i home.bmp,184,222\r",		"i homep.bmp,184,222\r",
	"i homek.bmp,184,222\r",	"i homepk.bmp,184,222\r",
	"i homec.bmp,184,222\r",	"i homepc.bmp,184,222\r",
	"i right.bmp,324,222\r",	"i rightp.bmp,324,222\r",
	"i plus.bmp,16,166\r",		"i plusp.bmp,16,166\r",
	"i minus.bmp,77,166\r", 	"i minusp.bmp,77,166\r",	
	"i plus.bmp,345,166\r", 		"i plusp.bmp,345,166\r",	
	"i minus.bmp,405,166\r",	"i minusp.bmp,405,166\r",
	//"i agmep.bmp,167,49\r",		"i agmeap.bmp,167,49\r",
	"i agme.png,186,49\r",		"i agmep.png,186,49\r",
	"i agmek.png,186,49\r",		"i agmepk.png,186,49\r",
	"i agme.png,186,49\r",		"i agmep.png,186,49\r",
	"i agseu.png,186,49\r",		"i agsed.png,186,49\r",
	"i agseuk.png,186,49\r",	"i agsedk.png,186,49\r",
	"i agseu.png,186,49\r",		"i agsed.png,186,49\r",
	//speed time setting
	"i left.bmp,44,222\r",		"i leftp.bmp,44,222\r",
	"i home.bmp,184,222\r",	"i homep.bmp,184,222\r",
	"i homek.bmp,184,222\r",	"i homepk.bmp,184,222\r",
	"i homec.bmp,184,222\r",	"i homepc.bmp,184,222\r",
	"i right.bmp,324,222\r",		"i rightp.bmp,324,222\r",
	"i plus.bmp,16,133\r",		"i plusp.bmp,16,133\r",
	"i minus.bmp,77,133\r", 	"i minusp.bmp,77,133\r",	
	"i plus.bmp,345,133\r", 		"i plusp.bmp,345,133\r",	
	"i minus.bmp,405,133\r",	"i minusp.bmp,405,133\r",
	//run
	"i left.bmp,44,222\r",		"i leftp.bmp,44,222\r",
	"i home.bmp,184,222\r",	"i homep.bmp,184,222\r",
	"i homek.bmp,184,222\r",	"i homepk.bmp,184,222\r",
	"i homec.bmp,184,222\r",	"i homepc.bmp,184,222\r",
	//"i right.bmp,324,222\r",	"i rightp.bmp,324,222\r",
	//"i user.bmp,10,2\r",		"i userp.bmp,10,2\r",
	"i opt.bmp,452,6\r",		"i optp.bmp,452,6\r",
	"i play.bmp,176,44\r",		"i playp.bmp,176,44\r",	  		//play
	"i pause.bmp,176,44\r",		"i pausep.bmp,176,44\r",		//playing
	"i play.bmp,176,44\r",		"i playp.bmp,176,44\r", 			//pause		
	"i stop.bmp,176,44\r",		"i stopp.bmp,176,44\r",			//Stop
	"\r",						"\r",
	"\r",						"\r",
	"\r",						"\r",
	//"i opt.bmp,290,6\r",		"i optp.bmp,290,6\r",	//pjg++190805 auto angle check button
	//"i p.bmp,235,1\r",		"i np.bmp,235,1\r",	//pjg++190805 auto angle play button
	//SystemSetup-main
	"i sycl.bmp,428,4\r",		"i psycl.bmp,428,4\r",
	"i susb.bmp,8,222\r",		"i susbp.bmp,8,222\r",
	"i susbk.bmp,8,222\r",		"i susbpk.bmp,8,222\r",
	"i susbc.bmp,8,222\r",		"i susbpc.bmp,8,222\r",
	"i adse.bmp,164,222\r",  	"i adsep.bmp,164,222\r",
	"i adsek.bmp,164,222\r",  	"i adsepk.bmp,164,222\r",
	"i adsec.bmp,164,222\r",  	"i adsepc.bmp,164,222\r",
	"i syin.bmp,319,222\r",		"i syinp.bmp,319,222\r",
	"i syink.bmp,319,222\r",	"i syinpk.bmp,319,222\r",
	"i syinc.bmp,319,222\r",		"i syinpc.bmp,319,222\r",
	//"\r",					"\r",
	//"i nsndchk.bmp,107,122\r", "i sndchk.bmp,107,122\r",
	"i psnd.bmp,428,114\r",		"i psndp.bmp,428,114\r",
	"i msnd.bmp,102,114\r",	"i msndp.bmp,102,114\r",
	//"\r",					"\r",
	//"i nbrchk.bmp,108,68\r",	"i brchk.bmp,108,68\r",
	"i pbr.bmp,428,60\r",		"i pbrp.bmp,428,60\r",
	"i mbr.bmp,102,60\r",		"i mbrp.bmp,102,60\r",
	"i ssko.bmp,126,169\r",		"i pssko.bmp,126,169\r", //kor
	"i ssen.bmp,223,169\r",		"i pssen.bmp,223,169\r", //eng
	"i ssch.bmp,379,169\r",		"i pssch.bmp,379,169\r", //china
	"i nbrchk.bmp,420,180\r",	"i nbrchk.bmp,420,180\r",  //quick
	//PiLoad
	"i left.bmp,44,222\r",		"i leftp.bmp,44,222\r",
	"i pidel.bmp,397,2\r",    	"i ppidel.bmp,397,2\r",
	"i pidelk.bmp,397,2\r",    	"i ppidelk.bmp,397,2\r",
	"i pidelc.bmp,397,2\r",    	"i ppidelc.bmp,397,2\r",
	"i usld.bmp,184,222\r", 	"i usldp.bmp,184,222\r",
	"i usldk.bmp,184,222\r", 	"i usldpk.bmp,184,222\r",
	"i usldc.bmp,184,222\r", 	"i usldpc.bmp,184,222\r",
	"i picl.bmp,444,2\r",    	"i ppicl.bmp,444,2\r",
	"i right.bmp,324,222\r",	"i rightp.bmp,324,222\r",
	#ifdef PI_LOAD_V1
	"i num1.bmp,142,49\r",		"i ppinum.bmp,142,49\r",
	"i num2.bmp,190,49\r",		"i ppinum.bmp,190,49\r",
	"i num3.bmp,238,49\r",		"i ppinum.bmp,238,49\r",
	"i num4.bmp,286,49\r",		"i ppinum.bmp,286,49\r",
	"i num5.bmp,334,49\r",		"i ppinum.bmp,334,49\r",
	"i num6.bmp,382,49\r",		"i ppinum.bmp,382,49\r",
	"i num7.bmp,430,49\r",		"i ppinum.bmp,430,49\r",
	"i num8.bmp,142,103\r",	"i ppinum.bmp,142,103\r",
	"i num9.bmp,190,103\r",	"i ppinum.bmp,190,103\r",
	"i num10.bmp,238,103\r",	"i ppinum.bmp,238,103\r",
	"i num11.bmp,286,103\r",	"i ppinum.bmp,286,103\r",
	#else
	"i num1.bmp,278,48\r",		"i ppinum.bmp,278,48\r",
	"i num2.bmp,326,48\r",		"i ppinum.bmp,326,48\r",
	"i num3.bmp,374,48\r",		"i ppinum.bmp,374,48\r",
	"i num4.bmp,422,48\r",		"i ppinum.bmp,422,48\r",
	"i num5.bmp,278,103\r",		"i ppinum.bmp,278,103\r",
	"i num6.bmp,326,103\r",		"i ppinum.bmp,326,103\r",
	"i num7.bmp,374,103\r",		"i ppinum.bmp,374,103\r",
	"i num8.bmp,422,103\r",		"i ppinum.bmp,422,103\r",
	"i num9.bmp,278,158\r",		"i ppinum.bmp,278,158\r",
	"i num0.bmp,326,158\r",		"i ppinum.bmp,326,158\r",
	"i pwdbc.bmp,374,158\r",	"i ppwdbc.bmp,374,158\r",
	#endif
	"i num12.bmp,334,103\r",	"i ppinum.bmp,334,103\r",
	"i num13.bmp,382,103\r",	"i ppinum.bmp,382,103\r",
	"i num14.bmp,430,103\r",	"i ppinum.bmp,430,103\r",
	"i num15.bmp,142,156\r",	"i ppinum.bmp,142,156\r",
	"i num16.bmp,190,156\r",	"i ppinum.bmp,190,156\r",
	"i num17.bmp,238,156\r",	"i ppinum.bmp,238,156\r",
	"i num18.bmp,286,156\r",	"i ppinum.bmp,286,156\r",
	"i num19.bmp,334,156\r",	"i ppinum.bmp,334,156\r",
	"i num20.bmp,382,156\r",	"i ppinum.bmp,382,156\r",
	"i num21.bmp,430,156\r",	"i ppinum.bmp,430,156\r",
	//
	"i pirsok.bmp,123,175\r",	"i ppirsok.bmp,123,175\r",
	"i pirsokk.bmp,123,175\r",	"i ppirsokk.bmp,123,175\r",
	"i pirsokc.bmp,123,175\r",	"i ppirsokc.bmp,123,175\r",
	"i usca.bmp,246,175\r",		"i uscap.bmp,246,175\r",
	"i uscak.bmp,246,175\r",	"i uscapk.bmp,246,175\r",
	"i uscac.bmp,246,175\r",	"i uscapc.bmp,246,175\r",
	//PiLoad2
	"i pidel.bmp,367,2\r",     	"i ppidel.bmp,367,2\r",
	"i pidelk.bmp,367,2\r",    	"i ppidelk.bmp,367,2\r",
	"i pidelc.bmp,367,2\r",    	"i ppidelc.bmp,367,2\r",
	"i left.bmp,44,222\r",		"i leftp.bmp,44,222\r",
	"i usld.bmp,184,222\r", 		"i usldp.bmp,184,222\r",
	"i usldk.bmp,184,222\r", 	"i usldpk.bmp,184,222\r",
	"i usldc.bmp,184,222\r", 	"i usldpc.bmp,184,222\r",
	"i picl.bmp,444,2\r",    		"i ppicl.bmp,444,2\r",
	"i right.bmp,324,222\r",		"i rightp.bmp,324,222\r",
	"i num22.bmp,142,49\r",	"i ppinum.bmp,142,49\r",
	"i num23.bmp,190,49\r",	"i ppinum.bmp,190,49\r",
	"i num24.bmp,238,49\r",	"i ppinum.bmp,238,49\r",
	"i num25.bmp,286,49\r",	"i ppinum.bmp,286,49\r",
	"i num26.bmp,334,49\r",	"i ppinum.bmp,334,49\r",
	"i num27.bmp,382,49\r",	"i ppinum.bmp,382,49\r",
	"i num28.bmp,430,49\r",	"i ppinum.bmp,430,49\r",
	"i num29.bmp,142,103\r",	"i ppinum.bmp,142,103\r",
	"i num30.bmp,190,103\r",	"i ppinum.bmp,190,103\r",
	"i num31.bmp,238,103\r",	"i ppinum.bmp,238,103\r",
	"i num32.bmp,286,103\r",	"i ppinum.bmp,286,103\r",
	"i num33.bmp,334,103\r",	"i ppinum.bmp,334,103\r",
	"i num34.bmp,382,103\r",	"i ppinum.bmp,382,103\r",
	"i num35.bmp,430,103\r",	"i ppinum.bmp,430,103\r",
	"i num36.bmp,142,156\r",	"i ppinum.bmp,142,156\r",
	"i num37.bmp,190,156\r",	"i ppinum.bmp,190,156\r",
	"i num38.bmp,238,156\r",	"i ppinum.bmp,238,156\r",
	"i num39.bmp,286,156\r",	"i ppinum.bmp,286,156\r",
	"i num40.bmp,334,156\r",	"i ppinum.bmp,334,156\r",
	"i num41.bmp,382,156\r",	"i ppinum.bmp,382,156\r",
	"i num42.bmp,430,156\r",	"i ppinum.bmp,430,156\r",
	//
	"i pirsok.bmp,122,150\r",	"i ppirsok.bmp,122,150\r",
	"i pirsokk.bmp,122,150\r",	"i ppirsokk.bmp,122,150\r",
	"i pirsokc.bmp,122,150\r",	"i ppirsokc.bmp,122,150\r",
	"i usca.bmp,245,150\r",		"i uscap.bmp,245,150\r",
	"i uscak.bmp,245,150\r",	"i uscapk.bmp,245,150\r",
	"i uscac.bmp,245,150\r",	"i uscapc.bmp,245,150\r",
	//PiLoad3
	"i pidel.bmp,367,2\r",     	"i ppidel.bmp,367,2\r",
	"i pidelk.bmp,367,2\r",    	"i ppidelk.bmp,367,2\r",
	"i pidelc.bmp,367,2\r",    	"i ppidelc.bmp,367,2\r",
	"i left.bmp,44,222\r",		"i leftp.bmp,44,222\r",
	"i usld.bmp,184,222\r", 		"i usldp.bmp,184,222\r",
	"i usldk.bmp,184,222\r", 	"i usldpk.bmp,184,222\r",
	"i usldc.bmp,184,222\r", 	"i usldpc.bmp,184,222\r",
	"i picl.bmp,444,2\r",    		"i ppicl.bmp,444,2\r",
	"i right.bmp,324,222\r",		"i rightp.bmp,324,222\r",
	"i num43.bmp,142,49\r",	"i ppinum.bmp,142,49\r",
	"i num44.bmp,190,49\r",	"i ppinum.bmp,190,49\r",
	"i num45.bmp,238,49\r",	"i ppinum.bmp,238,49\r",
	"i num46.bmp,286,49\r",	"i ppinum.bmp,286,49\r",
	"i num47.bmp,334,49\r",	"i ppinum.bmp,334,49\r",
	"i num48.bmp,382,49\r",	"i ppinum.bmp,382,49\r",
	"i num49.bmp,430,49\r",	"i ppinum.bmp,430,49\r",
	"i num50.bmp,142,103\r",	"i ppinum.bmp,142,103\r",
	"i num51.bmp,190,103\r",	"i ppinum.bmp,190,103\r",
	"i num52.bmp,238,103\r",	"i ppinum.bmp,238,103\r",
	"i num53.bmp,286,103\r",	"i ppinum.bmp,286,103\r",
	"i num54.bmp,334,103\r",	"i ppinum.bmp,334,103\r",
	"i num55.bmp,382,103\r",	"i ppinum.bmp,382,103\r",
	"i num56.bmp,430,103\r",	"i ppinum.bmp,430,103\r",
	"i num57.bmp,142,156\r",	"i ppinum.bmp,142,156\r",
	"i num58.bmp,190,156\r",	"i ppinum.bmp,190,156\r",
	"i num59.bmp,238,156\r",	"i ppinum.bmp,238,156\r",
	"i num60.bmp,286,156\r",	"i ppinum.bmp,286,156\r",
	"i num61.bmp,334,156\r",	"i ppinum.bmp,334,156\r",
	"i num62.bmp,382,156\r",	"i ppinum.bmp,382,156\r",
	"i num63.bmp,430,156\r",	"i ppinum.bmp,430,156\r",
	//
	"i pirsok.bmp,122,150\r",	"i ppirsok.bmp,122,150\r",
	"i pirsokk.bmp,122,150\r",	"i ppirsokk.bmp,122,150\r",
	"i pirsokc.bmp,122,150\r",	"i ppirsokc.bmp,122,150\r",
	"i usca.bmp,245,150\r",		"i uscap.bmp,245,150\r",
	"i uscak.bmp,245,150\r",	"i uscapk.bmp,245,150\r",
	"i uscac.bmp,245,150\r",	"i uscapc.bmp,245,150\r",
	//PiLoad4
	"i pidel.bmp,367,2\r",     	"i ppidel.bmp,367,2\r",
	"i pidelk.bmp,367,2\r",    	"i ppidelk.bmp,367,2\r",
	"i pidelc.bmp,367,2\r",    	"i ppidelc.bmp,367,2\r",
	"i left.bmp,44,222\r",		"i leftp.bmp,44,222\r",
	"i usld.bmp,184,222\r", 		"i usldp.bmp,184,222\r",
	"i usldk.bmp,184,222\r", 	"i usldpk.bmp,184,222\r",
	"i usldc.bmp,184,222\r", 	"i usldpc.bmp,184,222\r",
	"i picl.bmp,444,2\r",    		"i ppicl.bmp,444,2\r",
	//"i right.bmp,324,222\r",	"i rightp.bmp,324,222\r",
	"i num64.bmp,142,49\r",	"i ppinum.bmp,142,49\r",
	"i num65.bmp,190,49\r",	"i ppinum.bmp,190,49\r",
	"i num66.bmp,238,49\r",	"i ppinum.bmp,238,49\r",
	"i num67.bmp,286,49\r",	"i ppinum.bmp,286,49\r",
	"i num68.bmp,334,49\r",	"i ppinum.bmp,334,49\r",
	"i num69.bmp,382,49\r",	"i ppinum.bmp,382,49\r",
	"i num70.bmp,430,49\r",	"i ppinum.bmp,430,49\r",
	"i num71.bmp,142,103\r",	"i ppinum.bmp,142,103\r",
	"i num72.bmp,190,103\r",	"i ppinum.bmp,190,103\r",
	"i num73.bmp,238,103\r",	"i ppinum.bmp,238,103\r",
	"i num74.bmp,286,103\r",	"i ppinum.bmp,286,103\r",
	"i num75.bmp,334,103\r",	"i ppinum.bmp,334,103\r",
	"i num76.bmp,382,103\r",	"i ppinum.bmp,382,103\r",
	"i num77.bmp,430,103\r",	"i ppinum.bmp,430,103\r",
	"i num78.bmp,142,156\r",	"i ppinum.bmp,142,156\r",
	"i num79.bmp,190,156\r",	"i ppinum.bmp,190,156\r",
	"i num80.bmp,238,156\r",	"i ppinum.bmp,238,156\r",
	"i num81.bmp,286,156\r",	"i ppinum.bmp,286,156\r",
	"i num82.bmp,334,156\r",	"i ppinum.bmp,334,156\r",
	"i num83.bmp,382,156\r",	"i ppinum.bmp,382,156\r",
	"i num84.bmp,430,156\r",	"i ppinum.bmp,430,156\r",
	//
	"i pirsok.bmp,122,150\r",	"i ppirsok.bmp,122,150\r",
	"i pirsokk.bmp,122,150\r",	"i ppirsokk.bmp,122,150\r",
	"i pirsokc.bmp,122,150\r",	"i ppirsokc.bmp,122,150\r",
	"i usca.bmp,245,150\r",		"i uscap.bmp,245,150\r",
	"i uscak.bmp,245,150\r",	"i uscapk.bmp,245,150\r",
	"i uscac.bmp,245,150\r",	"i uscapc.bmp,245,150\r",
	//patient info
	"i padd.bmp,87,137\r",		"i padd.bmp,87,137\r",
	"i paca.bmp,240,135\r",		"i paca.bmp,240,135\r",
	//SystemSetup-progress
	//"i left.bmp,44,222\r",		"i leftp.bmp,44,222\r",
	"i adexit.bmp,184,222\r",	"i adexitp.bmp,184,222\r",
	"i adexitk.bmp,184,222\r",	"i adexitpk.bmp,184,222\r",
	"i adexitc.bmp,184,222\r",	"i adexitpc.bmp,184,222\r",
	"i right.bmp,324,222\r",		"i rightp.bmp,324,222\r",
	"i plus.bmp,286,66\r", 		"i plusp.bmp,286,66\r",	
	"i minus.bmp,345,66\r", 	"i minusp.bmp,345,66\r",
	"i plus.bmp,286,140\r", 		"i plusp.bmp,286,140\r",	
	"i minus.bmp,345,140\r", 	"i minusp.bmp,345,140\r",
	"\r",						"\r",
	//"i ndchk.jpg,239,4\r", 	"i dchk.jpg,239,4\r",
	// setup2-limit pause/sensitivity/home pos/mea sens/ang move step/auto angle value
	"i left.bmp,44,222\r",		"i leftp.bmp,44,222\r",
	//"i adexit.bmp,184,222\r",	"i adexitp.bmp,184,222\r",
	//"i adexitk.bmp,184,222\r",	"i adexitpk.bmp,184,222\r",
	//"i adexitc.bmp,184,222\r",	"i adexitpc.bmp,184,222\r",
	//"i right.bmp,324,222\r",		"i rightp.bmp,324,222\r",
	"i plus.bmp,286,102\r", 		"i plusp.bmp,286,102\r",	
	"i minus.bmp,345,102\r", 	"i minusp.bmp,345,102\r",
	"\r",						"\r",
	//"i ndchk.jpg,280,4\r", 	"i dchk.jpg,280,4\r",
    // setup3 vibration
    //"i left.bmp,44,222\r",		"i leftp.bmp,44,222\r",
	//"i adexit.bmp,184,222\r",	"i adexitp.bmp,184,222\r",
	//"i adexitk.bmp,184,222\r",	"i adexitpk.bmp,184,222\r",
	//"i adexitc.bmp,184,222\r",	"i adexitpc.bmp,184,222\r",
	//"i right.bmp,324,222\r",		"i rightp.bmp,324,222\r",
	"i plus.bmp,286,66\r", 		"i plusp.bmp,286,66\r",	
	"i minus.bmp,345,66\r", 	"i minusp.bmp,345,66\r",
	"i plus.bmp,286,140\r", 		"i plusp.bmp,286,140\r",	
	"i minus.bmp,345,140\r", 	"i minusp.bmp,345,140\r",
	"\r",						"\r",
	//"i ndchk.jpg,243,4\r", 	"i dchk.jpg,243,4\r",
	//setup4-sensitivity/home pos/mea sens/ang move step/auto angle value
	//"i left.bmp,44,222\r",		"i leftp.bmp,44,222\r",
	//"i adexit.bmp,184,222\r",	"i adexitp.bmp,184,222\r",
	//"i adexitk.bmp,184,222\r",	"i adexitpk.bmp,184,222\r",
	//"i adexitc.bmp,184,222\r",	"i adexitpc.bmp,184,222\r",
	//"i right.bmp,324,222\r",	"i rightp.bmp,324,222\r",
	//"i plus.bmp,286,102\r", 		"i plusp.bmp,286,102\r",	
	//"i minus.bmp,345,102\r", 	"i minusp.bmp,345,102\r",
	//SystemSetup-USB save
	"i ussa.bmp,122,222\r",		"i ussap.bmp,122,222\r",
	"i ussak.bmp,122,222\r",	"i ussapk.bmp,122,222\r",
	"i ussac.bmp,122,222\r",	"i ussapc.bmp,122,222\r",
	"i usca.bmp,245,222\r",		"i uscap.bmp,245,222\r",
	"i uscak.bmp,245,222\r",	"i uscapk.bmp,245,222\r",
	"i uscac.bmp,245,222\r",	"i uscapc.bmp,245,222\r",
	//SystemSetup-version
	//"i siup.bmp,122,186\r",  	"i siupp.bmp,122,186\r",
	"i clo.bmp,389,30\r",  		"i pclo.bmp,389,30\r",
	"i sirs.bmp,107,190\r",  	"i psirs.bmp,107,190\r",
	"i sirsk.bmp,107,190\r",  	"i psirsk.bmp,107,190\r",
	"i sirsc.bmp,107,190\r",  	"i psirsc.bmp,107,190\r",
	"i defaultu.bmp,264,190\r", "i defaultd.bmp,264,190\r",
	"i defaultu.bmp,264,190\r", "i defaultd.bmp,264,190\r",
	"i defaultu.bmp,264,190\r", "i defaultd.bmp,264,190\r",
	//SystemSetup-Reset
	//"i sirs.bmp,122,150\r",		"i psirs.bmp,122,150\r",
	"i pirsok.bmp,122,150\r",	"i ppirsok.bmp,122,150\r",		//ok eng
	"i pirsokk.bmp,122,150\r",	"i ppirsokk.bmp,122,150\r",		//ok kor
	"i pirsokc.bmp,122,150\r",	"i ppirsokc.bmp,122,150\r",		//ok china
	"i pirsokn.bmp,122,150\r",	"",								//ok disable eng
	"i pirsokn.bmp,122,150\r",	"",		
	"i pirsokn.bmp,122,150\r",	"",		
	"i usca.bmp,245,150\r",		"i uscap.bmp,245,150\r",		//no eng
	"i uscak.bmp,245,150\r",	"i uscapk.bmp,245,150\r",
	"i uscac.bmp,245,150\r",	"i uscapc.bmp,245,150\r",
	//"i siall.bmp,57,150\r",		"i siallp.bmp,57,150\r",
	//"i side.bmp,182,150\r",		"i sidep.bmp,182,150\r",
	//"i usca.bmp,310,150\r",		"i uscap.bmp,310,150\r",
	//training complete
	"i pirsok.bmp,175,171\r",	"i ppirsok.bmp,175,171\r",		//yes
	"i pirsokk.bmp,175,171\r",	"i ppirsokk.bmp,175,171\r",		//yes
	"i pirsokc.bmp,175,171\r",	"i ppirsokc.bmp,175,171\r",		//yes
	"i fogno.bmp,200,191\r",	"i pfogno.bmp,200,191\r",		//no
	//init patient info
	//"i iiyu.bmp,141,153\r",		"i iiyd.bmp,141,153\r",			//ok
	//"i iinu.bmp,260,153\r",		"i iind.bmp,260,153\r",			//no
	//measure complete
	"i pirsok.bmp,117,150\r",	"i ppirsok.bmp,117,150\r",
	"i pirsokk.bmp,117,150\r",	"i ppirsokk.bmp,117,150\r",
	"i pirsokc.bmp,117,150\r",	"i ppirsokc.bmp,117,150\r",
	"i retryu.bmp,277,150\r",	"i retryd.bmp,277,150\r",
	"i retryuk.bmp,277,150\r",	"i retrydk.bmp,277,150\r",
	"i retryu.bmp,277,150\r",	"i retryd.bmp,277,150\r",
	//adv set main
	"i aprou.bmp,9,68\r",		"i aprod.bmp,9,68\r",
	"i alpu.bmp,9,119\r",		"i alpd.bmp,9,119\r",
	"i avibu.bmp,9,169\r",		"i avibd.bmp,9,169\r",
	"i asensu.bmp,9,219\r",		"i asensd.bmp,9,219\r",
	"i asensuk.bmp,9,219\r",	"i asensdk.bmp,9,219\r",
	"i asensu.bmp,9,219\r",		"i asensd.bmp,9,219\r",
	"i aipu.bmp,246,68\r",		"i aipd.bmp,246,68\r",
	"i aipuk.bmp,246,68\r",		"i aipdk.bmp,246,68\r",
	"i aipu.bmp,246,68\r",		"i aipd.bmp,246,68\r",
	"i amsu.bmp,246,119\r",		"i amsd.bmp,246,119\r",
	"i amsuk.bmp,246,119\r",	"i amsdk.bmp,246,119\r",
	"i amsu.bmp,246,119\r",		"i amsd.bmp,246,119\r",
	"i aamsu.bmp,246,169\r",	"i aamsd.bmp,246,169\r",
	"i aamsuk.bmp,246,169\r",	"i aamsdk.bmp,246,169\r",
	"i aamsu.bmp,246,169\r",	"i aamsd.bmp,246,169\r",
	"i aaasu.bmp,246,219\r",	"i aaasd.bmp,246,219\r",	
	"i aaasuk.bmp,246,219\r",	"i aaasdk.bmp,246,219\r",	
	"i aaasu.bmp,246,219\r",	"i aaasd.bmp,246,219\r",	
};

char *pPatientInfoTxt[][LT_MAX] = {						//»ç¿ë ¾ÈÇÔ 
	// eng                                    kor                               china
	"i pipit.bmp,11,6\r"	, 		"i pipiet.bmp,11,6\r", 		"i pipict.bmp,11,6\r",
	"i pipidt.bmp,120,130\r",	"i pipidet.bmp,88,130\r", 	"i pipidct.bmp,120,130\r",
	//"i rnpid.bmp,40,220\r",	"i rnpide.bmp,30,220\r",		"i srnpidc.bmp,40,220\r",
	"i rnttt.bmp,30,220\r",		"i rnttte.bmp,30,220\r"	,	"i rntttc.bmp,25,220\r",
	"i rntct.bmp,120,220\r",		"i rntcte.bmp,117,220\r"	,	"i rntctc.bmp,117,220\r",
};

char *pMultiLangImg[] = {
	"i lkord.bmp,235,82\r",
	"i lengd.bmp,235,82\r",
	"i lchid.bmp,235,82\r"          
};

char *pLdBkgImg = {"i ldbg.bmp,0,0\r"};
char *pSndInfo[][LT_MAX] = {	
	//eng                               kor                         china   etc..
	{"sp wav/1E.wav\r",	 	"sp wav/1.wav\r",  	"sp wav/1c.wav\r"},		//angle
	{"sp wav/2E.wav\r",		 "sp wav/2.wav\r",  	"sp wav/2c.wav\r"},		//go home
	{"sp wav/3.wav\r",		 "sp wav/3.wav\r",  	"sp wav/3.wav\r"},		// go first button
	{"sp wav/4E.wav\r",		 "sp wav/4.wav\r",  	"sp wav/4c.wav\r"},		//run stop
	{"sp wav/5E.wav\r",		 "sp wav/5.wav\r",  	"sp wav/5c.wav\r"},		//run start
	{"sp wav/6E.wav\r",		 "sp wav/6.wav\r",  	"sp wav/6c.wav\r"},		//run pause
	{"sp wav/7E.wav\r",		 "sp wav/7.wav\r",  	"sp wav/7c.wav\r"},		//patient info
	{"sp wav/8.wav\r", 		 "sp wav/8.wav\r",  	"sp wav/8.wav\r"},		//left/right button
	{"sp wav/9E.wav\r",		 "sp wav/9.wav\r",  	"sp wav/9c.wav\r"},		//speed/time
	{"sp wav/10E.wav\r",		 "sp wav/10.wav\r", 	"sp wav/10c.wav\r"},		//init system
	{"sp wav/11.wav\r",		 "sp wav/11.wav\r", 	"sp wav/11.wav\r"},		//up/down button
	{"sp wav/12E.wav\r", 		 "sp wav/12.wav\r", 	"sp wav/12c.wav\r"},		//motor overload
	{"sp wav/13.wav\r", 		 "sp wav/13.wav\r", 	"sp wav/13.wav\r"},		//go to setup
	{"sp wav/14E.wav\r", 	 	 "sp wav/14.wav\r", 	"sp wav/14c.wav\r"},		//train complete
	{"sp wav/15E.wav\r\0", 	 "sp wav/15.wav\r\0", 	"sp wav/15c.wav\r\0"},		//system error
	{"sp wav/16E.wav\r\0", 	 "sp wav/16E.wav\r\0", 	"sp wav/16E.wav\r\0"},		//calibration complete
};	

char *pWndBgNameInfo[][LT_MAX] = {		//window background file name
	//eng								//kor							//chi
	{"i inpo.bmp,0,0\r",				"i inpok.bmp,0,0\r",				"i inpoc.bmp,0,0\r"},			//init
	{"i load.bmp,0,0\r",				"i loadk.bmp,0,0\r",				"i loadc.bmp,0,0\r"},			//loading
	{"i homebg.bmp,0,0\r",				"i homebgk.bmp,0,0\r", 				"i homedg.bmp,0,0\r"},			//home
	{"i userbg.bmp,0,0\r",				"i userbgk.bmp,0,0\r",				"i userbg.bmp,0,0\r"},			//user select
	{"i mea3.bmp,0,0\r",				"i mea3k.bmp,0,0\r",				"i mea3.bmp,0,0\r"},			//angle measure
	{"i angb.bmp,0,0\r",				"i angbk.bmp,0,0\r",				"i angbc.bmp,0,0\r"},			//angle
	{"i sptmb.bmp,0,0\r",				"i sptmbk.bmp,0,0\r",				"i sptmbc.bmp,0,0\r"},			//speed/time
	{"i runb.bmp,0,0\r",				"i runbk.bmp,0,0\r",				"i runbc.bmp,0,0\r"},			//run
	{"i sysbg.bmp,0,0\r",				"i sysbgk.bmp,0,0\r",				"i sysbgc.bmp,0,0\r"},			//setup
	{"i usscbg.bmp,0,0\r",				"i usscbgk.bmp,0,0\r",				"i usscbg.bmp,0,0\r"},			//USER select
	{"i asetup.bmp,0,0\r",				"i asetup.bmp,0,0\r",				"i asetup.bmp,0,0\r"},			//advance setting
	{"i prob.bmp,0,0\r",				"i probk.bmp,0,0\r",				"i probc.bmp,0,0\r"},			//Progress È­¸é
	{"i limb.bmp,0,0\r",				"i limbk.bmp,0,0\r",				"i limbc.bmp,0,0\r"},			//Limited Pause È­¸é
	{"i vibb.bmp,0,0\r",				"i vibbk.bmp,0,0\r",				"i vibbc.bmp,0,0\r"},			//vibration È­¸é
	{"i asens.bmp,0,0\r",				"i asensk.bmp,0,0\r",				"i asens.bmp,0,0\r"},			//sensitivy
	{"i aiposi.bmp,0,0\r",				"i aiposik.bmp,0,0\r",				"i aiposi.bmp,0,0\r"},			//home pos
	{"i amsens.bmp,0,0\r",				"i amsensk.bmp,0,0\r",				"i amsens.bmp,0,0\r"},			//mea sens
	{"i aamstep.bmp,0,0\r",				"i aamstepk.bmp,0,0\r",				"i aamstep.bmp,0,0\r"},			//angle move step
	{"i aaaset.bmp,0,0\r",				"i aaasetk.bmp,0,0\r",				"i aaaset.bmp,0,0\r"},			//auto angle setting value
	//popup
	{"i susbb.png,0,0\r",				"i susbbk.png,0,0\r",				"i susbbc.png,0,0\r"},			//USB save req
	{"i Saving.png,0,0\r",				"i Savingk.png,0,0\r",				"i Savingc.png,0,0\r"},			//Saving
	{"i sifbg.png,0,0\r",				"i sifbgk.png,0,0\r",				"i sifbg.png,0,0\r"},			//system info
	{"i usbrc.png,0,0\r",				"i usbrck.png,0,0\r",				"i usbrcc.png,0,0\r"},			//USB check
	{"i traincom.png,0,0\r",			"i traincomk.png,0,0\r",			"i traincom.png,0,0\r"},		//run complete popup
	{"i resbg.png,0,0\r",				"i resbgk.png,0,0\r",				"i resbg.png,0,0\r"},			//Reset ÆË¾÷//default set param
	{"i deldata.png,0,0\r",				"i deldatak.png,0,0\r",				"i deldata.png,0,0\r"},			//Delete user data
	{"i newreg.png,0,0\r",				"i newregk.png,0,0\r",				"i newreg.png,0,0\r"},			//user create
	{"i idcheck.png,0,0\r",				"i idcheckk.png,0,0\r",				"i idcheck.png,0,0\r"},			//id check
	{"i pirspu.png,0,0\r",				"i pirspuk.png,0,0\r",				"i pirspuc.png,0,0\r"},			//USER data delete popup
	{"i mearesult.png,0,0\r",			"i mearesultk.png,0,0\r",			"i mearesult.png,0,0\r"},		//Angle Mea Report
	{"i mea1st.png,0,0\r",				"i mea1stk.png,0,0\r",				"i mea1st.png,0,0\r"},			//Angle Mea complete1
	{"i mea2nd.png,0,0\r",				"i mea2ndk.png,0,0\r",				"i mea2nd.png,0,0\r"},			//Angle Mea complete2
	{"i mea3rd.png,0,0\r",				"i mea3rdk.png,0,0\r",				"i mea3rd.png,0,0\r"},			//Angle Mea complete3
	{"i meaend.png,0,0\r",				"i meaendk.png,0,0\r",				"i meaend.png,0,0\r"},			//Angle Mea complete
	{"i aprogress.png,0,0\r",			"i aprogressk.png,0,0\r",			"i aprogress.png,0,0\r"},		//progress on message
	{"i daprogress.png,0,0\r",			"i daprogressk.png,0,0\r",			"i daprogress.png,0,0\r"},		//progress off message
	{"i alimpos.png,0,0\r",				"i alimposk.png,0,0\r",				"i alimpos.png,0,0\r"},			//limit on message
	{"i dalimpos.png,0,0\r",			"i dalimposk.png,0,0\r",			"i dalimpos.png,0,0\r"},		//limit off message
	{"i avibr.png,0,0\r",				"i avibrk.png,0,0\r",				"i avibr.png,0,0\r"},			//vibration on message
	{"i davibr.png,0,0\r",				"i davibrk.png,0,0\r",				"i davibr.png,0,0\r"},			//vibration off message
	{"i warme.png,0,0\r",				"i warmek.png,0,0\r",				"i warme.png,0,0\r"},			//memory usage
	{"i warmfull.png,0,0\r",			"i warmfullk.png,0,0\r",			"i warmfull.png,0,0\r"},		//memory full	
	{"i restbg.png,0,0\r",				"i restbgk.png,0,0\r",				"i restbg.png,0,0\r"},			//reseting	
};

char *pImgNameInfo[][LT_MAX] = {		//image file name
	//eng								//kor								//chi
	{"i agseno.png,186,49\r",			"i agsenok.png,186,49\r",			"i agseno.png,186,49\r"},	//rehab button disable	
	{"i agseu.png,186,49\r",			"i agseuk.png,186,49\r",			"i agseu.png,186,49\r"},	//hehab button enable
	{"i agme.png,186,49\r", 			"i agmek.png,186,49\r", 			"i agme.png,186,49\r"},
	{"i Error.png,0,0\r", 				"i Errork.png,0,0\r", 				"i error.png,0,0\r"},


};

/* Private function prototypes -----------------------------------------------*/


////////////////////////////Function////////////////////////////////////////


