/*
 *
 * NMEA library
 * URL: http://nmea.sourceforge.net
 * Author: Tim (xtimor@gmail.com)
 * Licence: http://www.gnu.org/licenses/lgpl.html
 * $Id: parse.h 4 2007-08-27 13:11:03Z xtimor $
 *
 */

#ifndef __NMEA_PARSE_H__
#define __NMEA_PARSE_H__

#include "sentence.h"

#ifdef  __cplusplus
extern "C" {
#endif

int nmea_pack_type(const char *buff, int buff_sz);
int nmea_find_tail(const char *buff, int buff_sz, int *res_crc);

int nmea_parse_GPGGA(const char *buff, int buff_sz, nmeaGPGGA *pack);
int nmea_parse_GPGSA(const char *buff, int buff_sz, nmeaGPGSA *pack);
int nmea_parse_GPGSV(const char *buff, int buff_sz, nmeaGPGSV *pack);
int nmea_parse_GPRMC(const char *buff, int buff_sz, nmeaGPRMC *pack);
int nmea_parse_GPVTG(const char *buff, int buff_sz, nmeaGPVTG *pack);
	
int nmea_parse_GNGGA(const char *buff, int buff_sz, nmeaGNGGA *pack);
int nmea_parse_GNRMC(const char *buff, int buff_sz, nmeaGNRMC *pack);
int nmea_parse_GNVTG(const char *buff, int buff_sz, nmeaGNVTG *pack);
int nmea_parse_GNZDA(const char *buff, int buff_sz, nmeaGNZDA *pack);
int nmea_parse_GNGLL(const char *buff, int buff_sz, nmeaGNGLL *pack);
	
int nmea_parse_BDGSV(const char *buff, int buff_sz, nmeaBDGSV *pack);
int nmea_parse_BDGSA(const char *buff, int buff_sz, nmeaBDGSA *pack);
int nmea_parse_GPTXT(const char *buff, int buff_sz, nmeaGPTXT *pack);

void nmea_GPGGA2info(nmeaGPGGA *pack, nmeaINFO *info);
void nmea_GPGSA2info(nmeaGPGSA *pack, nmeaINFO *info);
void nmea_GPGSV2info(nmeaGPGSV *pack, nmeaINFO *info);
void nmea_GPRMC2info(nmeaGPRMC *pack, nmeaINFO *info);
void nmea_GPVTG2info(nmeaGPVTG *pack, nmeaINFO *info);

void nmea_GNGGA2info(nmeaGNGGA *pack, nmeaINFO *info);
void nmea_GNRMC2info(nmeaGNRMC *pack, nmeaINFO *info);
void nmea_GNVTG2info(nmeaGNVTG *pack, nmeaINFO *info);
void nmea_GNZDA2info(nmeaGNZDA *pack, nmeaINFO *info);
void nmea_GNGLL2info(nmeaGNGLL *pack, nmeaINFO *info);

void nmea_BDGSV2info(nmeaBDGSV *pack, nmeaINFO *info);
void nmea_BDGSA2info(nmeaBDGSA *pack, nmeaINFO *info);
void nmea_GPTXT2info(nmeaGPTXT *pack, nmeaINFO *info);

#ifdef  __cplusplus
}
#endif

#endif /* __NMEA_PARSE_H__ */
