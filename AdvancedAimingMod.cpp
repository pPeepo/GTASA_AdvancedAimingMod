#include "shared/AML/amlmod.h"
#include "GTASA/common.h"
#include "shared/CEvents.h"

MYMOD(net.peepo.AdvancedAimingMod, AdvancedAimingMod, 1.1, GTAPLUGIN)

enum eWeaponAimOffset {
    AIM_OFFSET_WEAPON_DEFAULT,
    AIM_OFFSET_WEAPON_COLT45,
    AIM_OFFSET_WEAPON_SILENCED,
    AIM_OFFSET_WEAPON_DESERTEAGLE,
    AIM_OFFSET_WEAPON_CHROMEGUN,
    AIM_OFFSET_WEAPON_SAWNOFF,
    AIM_OFFSET_WEAPON_SHOTGSPA,
    AIM_OFFSET_WEAPON_MICROUZI,
    AIM_OFFSET_WEAPON_MP5,
    AIM_OFFSET_WEAPON_TEC9,
    AIM_OFFSET_WEAPON_AK47,
    AIM_OFFSET_WEAPON_M4,
    AIM_OFFSET_WEAPON_CUNTGUN,
    AIM_OFFSET_WEAPON_ROCKETLA,
    AIM_OFFSET_WEAPON_HEATSEEK,
    AIM_OFFSET_WEAPON_FLAME
};

struct tAimingCamData {
    float f0, f1, f2, f3, f4, f5, f6;
};

static tAimingCamData gData[4];
static CVector gOffsets[16];
static unsigned int updateSets;
static bool rightSide = false;

void ReadSettings() {
    char line[512];
    char path[128];
    char dummy[128];
    float fVal;
    sprintf(path, "%sdata/aimingSets.dat", aml->GetAndroidDataPath());
    FILE *file = fopen(path, "r");
    fgets(line, 512, file);
    sscanf(line, "%s %d", dummy, &updateSets);
    fgets(line, 512, file);
    fgets(line, 512, file);
    sscanf(line, "%s %d", dummy, &rightSide);
    fgets(line, 512, file);
    sscanf(line, "%s %f", dummy, (float *)(libs.pGame + 0x6A9F40));
    fgets(line, 512, file);
    sscanf(line, "%s %f", dummy, (float *)(libs.pGame + 0x6A9F44));
    fgets(line, 512, file);
    sscanf(line, "%s %f", dummy, &fVal);
    CPatch::SetFloat(libs.pGame + 0x3C6F3C, fVal);
    fgets(line, 512, file);
    sscanf(line, "%s %f", dummy, (float *)(libs.pGame + 0x6A9F3C));
    fgets(line, 512, file);
    sscanf(line, "%s %f", dummy, (float *)(libs.pGame + 0x6A9F38));
    fgets(line, 512, file);
    sscanf(line, "%s %f", dummy, (float *)(libs.pGame + 0x6A9F34));
    fgets(line, 512, file);
    sscanf(line, "%s %f", dummy, (float *)(libs.pGame + 0x6A9F30));
    fgets(line, 512, file);
    for (int i = 0; i < 4; i++) {
    	fgets(line, 512, file);
    	sscanf(line, "%s %f %f %f %f %f %f %f", dummy, &gData[i].f0, &gData[i].f1, &gData[i].f2, &gData[i].f3, &gData[i].f4,
            &gData[i].f5, &gData[i].f6);
    }
    memcpy((void *)(libs.pGame + 0x6A9F48), gData, 112);
    fgets(line, 512, file);
    for (int i = 0; i < 16; i++) {
        fgets(line, 512, file);
        sscanf(line, "%s %f %f %f", dummy, &gOffsets[i].x, &gOffsets[i].y, &gOffsets[i].z);
    }
    fclose(file);
}

DECL_HOOKv(MyProcess_AimWeapon, CCam *cam, CVector const &vec, float arg3, float arg4, float arg5) 
{
    if (updateSets) ReadSettings();
	
    CPed *playa = FindPlayerPed();
    if (playa && !playa->m_nPedFlags.bInVehicle) {
        int aimTypeId = 0;
        switch (playa->m_aWeapons[playa->m_nActiveWeaponSlot].m_nType) {
        case WEAPON_PISTOL: 
            aimTypeId = AIM_OFFSET_WEAPON_COLT45;
            break;
        case WEAPON_PISTOL_SILENCED:
            aimTypeId = AIM_OFFSET_WEAPON_SILENCED;
            break;
        case WEAPON_DESERT_EAGLE:
            aimTypeId = AIM_OFFSET_WEAPON_DESERTEAGLE;
            break;
        case WEAPON_SHOTGUN:
            aimTypeId = AIM_OFFSET_WEAPON_CHROMEGUN;
            break;
        case WEAPON_SAWNOFF:
            aimTypeId = AIM_OFFSET_WEAPON_SAWNOFF;
            break;
        case WEAPON_SPAS12:
            aimTypeId = AIM_OFFSET_WEAPON_SHOTGSPA;
            break;
        case WEAPON_MICRO_UZI:
            aimTypeId = AIM_OFFSET_WEAPON_MICROUZI;
            break;
        case WEAPON_MP5:
            aimTypeId = AIM_OFFSET_WEAPON_MP5;
            break;
        case WEAPON_TEC9:
            aimTypeId = AIM_OFFSET_WEAPON_TEC9;
            break;
        case WEAPON_AK47:
            aimTypeId = AIM_OFFSET_WEAPON_AK47;
            break;
        case WEAPON_M4:
            aimTypeId = AIM_OFFSET_WEAPON_M4;
            break;
        case WEAPON_COUNTRYRIFLE:
            aimTypeId = AIM_OFFSET_WEAPON_CUNTGUN;
            break;
        case WEAPON_ROCKET:
            aimTypeId = AIM_OFFSET_WEAPON_ROCKETLA;
            break;
        case WEAPON_ROCKET_HS:
            aimTypeId = AIM_OFFSET_WEAPON_HEATSEEK;
            break;
        case WEAPON_FTHROWER:
            aimTypeId = AIM_OFFSET_WEAPON_FLAME;
            break;
        }
        if (aimTypeId != -1) {
            CVector offset = gOffsets[aimTypeId];
			
            // if rightSide flag enabled
            if (rightSide)
                offset.x *= -3.0f;
            
            return MyProcess_AimWeapon(cam, playa->TransformFromObjectSpace(offset), arg3, arg4, arg5);
        }
        else 
            return MyProcess_AimWeapon(cam, vec, arg3, arg4, arg5);
    }
    return MyProcess_AimWeapon(cam, vec, arg3, arg4, arg5);
}

extern "C" void OnModLoad()
{
    ALOGI(GTAPLUGIN, "libAdvancedAimingMod.so init...");
	
    HOOKPLT(MyProcess_AimWeapon, libs.pGame + 0x66E748);
    ReadSettings();
}