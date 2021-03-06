
#include "dcpuhw.h"

struct EEROM_rvstate {
	uint16_t sz;
};
ISIREFLECT(struct EEROM_rvstate,
	ISIR(EEROM_rvstate, uint16_t, sz)
)

static int EEROM_SIZE(int t, const uint8_t *cfg, size_t lcfg, size_t *sz)
{
	uint32_t rqs = 0;
	uint64_t mid = 0;
	char * fname = 0;
	rqs = 0;
	if(!isi_fetch_parameter(cfg, lcfg, 2, &mid, sizeof(uint64_t)) && mid) {
		isi_find_bin(mid, &fname);
	}
	if(isi_fetch_parameter(cfg, lcfg, 1, &rqs, sizeof(uint32_t))) {
		/* pick a default if we don't get the option */
		if(!fname) {
			rqs = 2048;
		} else {
			rqs = isi_fsize(fname);
		}
	}
	if(rqs > 0x20000) rqs = 0x20000;
	if(fname) free(fname);
	switch(t) {
	case 0: return *sz = sizeof(struct EEROM_rvstate) + rqs, 0;
	default: return 0;
	}
}

static int EEROM_Init(struct isiInfo *info, const uint8_t *cfg, size_t lcfg);
static struct isidcpudev EEROM_Meta = {0x0000,0x17400011,MF_ECIV};
static struct isiConstruct EEROM_Con = {
	ISIT_HARDWARE, "rom", "Embedded ROM",
	0, EEROM_Init, EEROM_SIZE,
	&ISIREFNAME(struct EEROM_rvstate), NULL,
	&EEROM_Meta
};
void EEROM_Register()
{
	isi_register(&EEROM_Con);
}

static int EEROM_Reset(struct isiInfo *info, struct isiInfo *host, struct timespec mtime)
{
	if(!info->mem) return 1;
	if(!info->rvstate) return 1;
	memcpy(
		((isiram16)info->mem)->ram,
		((struct EEROM_rvstate *)info->rvstate)+1,
		((struct EEROM_rvstate *)info->rvstate)->sz << 1
	      );
	return 0;
}

static int EEROM_Query(struct isiInfo *info, struct isiInfo *src, uint16_t *msg, struct timespec crun)
{
	msg[2] = ((struct EEROM_rvstate *)info->rvstate)->sz;
	return 0;
}

static int EEROM_HWI(struct isiInfo *info, struct isiInfo *src, uint16_t *msg, struct timespec crun)
{
	switch(msg[0]) {
	case 0:
		memcpy(
			((isiram16)info->mem)->ram,
			((struct EEROM_rvstate *)info->rvstate)+1,
			((struct EEROM_rvstate *)info->rvstate)->sz << 1
		      );
		break;
	case 1:
		memcpy(
			((struct EEROM_rvstate *)info->rvstate)+1,
			((isiram16)info->mem)->ram,
			((struct EEROM_rvstate *)info->rvstate)->sz << 1
		      );
		break;
	}
	return 0;
}

static int EEROM_MsgIn(struct isiInfo *info, struct isiInfo *src, uint16_t *msg, struct timespec mtime)
{
	switch(msg[0]) {
	case 0: return EEROM_Reset(info, src, mtime);
	case 1: return EEROM_Query(info, src, msg+2, mtime);
	case 2: return EEROM_HWI(info, src, msg+2, mtime);
	default: break;
	}
	return 1;
}

static int EEROM_Init(struct isiInfo *info, const uint8_t * cfg, size_t lcfg)
{
	info->MsgIn = EEROM_MsgIn;
	uint32_t rqs = 0;
	uint64_t mid = 0;
	uint8_t le = 0;
	char * fname = 0;
	struct EEROM_rvstate *rvrom = (struct EEROM_rvstate *)info->rvstate;
	rqs = 0;
	if(!isi_fetch_parameter(cfg, lcfg, 2, &mid, sizeof(uint64_t)) && mid) {
		isi_find_bin(mid, &fname);
	}
	if(isi_fetch_parameter(cfg, lcfg, 1, &rqs, sizeof(uint32_t))) {
		/* pick a default if we don't get the option */
		if(!fname) {
			rqs = 2048;
		} else {
			rqs = isi_fsize(fname);
		}
	}
	if(!isi_fetch_parameter(cfg, lcfg, 3, &le, 1)) {
		le = 1;
	}
	if(rqs > 0x20000) rqs = 0x20000;
	rvrom->sz = rqs >> 1;
	if(fname) {
		loadbinfileto(fname, le, (uint8_t*)(rvrom+1), rqs);
		free(fname);
	}
	return 0;
}

