
#include "nmea/sentence.h"

#include <string.h>

void nmea_zero_GPGGA(nmeaGPGGA *pack)
{
    memset(pack, 0, sizeof(nmeaGPGGA));
    nmea_time_now(&pack->utc);
    pack->ns = 'N';
    pack->ew = 'E';
    pack->elv_units = 'M';
    pack->diff_units = 'M';
}

void nmea_zero_GPGSA(nmeaGPGSA *pack)
{
    memset(pack, 0, sizeof(nmeaGPGSA));
    pack->fix_mode = 'A';
    pack->fix_type = NMEA_FIX_BAD;
}

void nmea_zero_GPGSV(nmeaGPGSV *pack)
{
    memset(pack, 0, sizeof(nmeaGPGSV));
}

void nmea_zero_GPRMC(nmeaGPRMC *pack)
{
    memset(pack, 0, sizeof(nmeaGPRMC));
    nmea_time_now(&pack->utc);
    pack->status = 'V';
    pack->ns = 'N';
    pack->ew = 'E';
    pack->declin_ew = 'E';
}

void nmea_zero_GPVTG(nmeaGPVTG *pack)
{
    memset(pack, 0, sizeof(nmeaGPVTG));
    pack->dir_t = 'T';
    pack->dec_m = 'M';
    pack->spn_n = 'N';
    pack->spk_k = 'K';
}

void nmea_zero_GNGGA(nmeaGNGGA *pack)
{
    memset(pack, 0, sizeof(nmeaGNGGA));
    nmea_time_now(&pack->utc);
    pack->uLat = 'N';
    pack->uLon = 'E';
    pack->uMsl = 'M';
    pack->uSep = 'M';
}

void nmea_zero_GNRMC(nmeaGNRMC *pack)
{
    memset(pack, 0, sizeof(nmeaGNRMC));
    nmea_time_now(&pack->utc);
    pack->status = 'V';
    pack->uLat = 'N';
    pack->uLon = 'E';
    pack->mvE = 'E';
}

void nmea_zero_GNVTG(nmeaGNVTG *pack)
{
    memset(pack, 0, sizeof(nmeaGNVTG));
    pack->T = 'T';
    pack->M = 'M';
    pack->N = 'N';
    pack->K = 'K';
}

void nmea_zero_GNZDA(nmeaGNZDA *pack)
{
    memset(pack, 0, sizeof(nmeaGNZDA));
    nmea_time_now(&pack->utc);
}

void nmea_zero_GNGLL(nmeaGNGLL *pack)
{
    memset(pack, 0, sizeof(nmeaGNGLL));
    nmea_time_now(&pack->utc);
    pack->Value = 'V';
    pack->uLat = 'N';
    pack->uLon = 'E';
}

void nmea_zero_BDGSA(nmeaBDGSA *pack)
{
    memset(pack, 0, sizeof(nmeaBDGSA));
    pack->Smode = 'A';
    pack->FS = NMEA_FIX_BAD;
}

void nmea_zero_GPTXT(nmeaGPTXT *pack)
{
    memset(pack, 0, sizeof(nmeaGPTXT));

}
