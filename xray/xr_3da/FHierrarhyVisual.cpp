// FHierrarhyVisual.cpp: implementation of the FHierrarhyVisual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "FHierrarhyVisual.h"
#include "fmesh.h"
#ifndef _EDITOR
 #include "render.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FHierrarhyVisual::FHierrarhyVisual()  : IRender_Visual()
{
	bDontDelete	= FALSE;
}

FHierrarhyVisual::~FHierrarhyVisual()
{
	if (!bDontDelete) {
		for (u32 i=0; i<children.size(); i++)	
			::Render->model_Delete(children[i]);
	}
	children.clear();
}

void FHierrarhyVisual::Release()
{
	if (!bDontDelete) {
		for (u32 i=0; i<children.size(); i++)
			children[i]->Release();
	}
}

void FHierrarhyVisual::Load(const char* N, IReader *data, u32 dwFlags)
{
	CTimer perf;
	perf.Start();

	static u64 total_loaded;
	size_t chsz = data->length();

	IRender_Visual::Load(N,data,dwFlags);
	if (data->find_chunk(OGF_CHILDREN_L)) 
	{
		// From Link
		u32 cnt = data->r_u32		();
		children.resize				(cnt);
		for (u32 i=0; i<cnt; i++)	{
#ifdef _EDITOR
			THROW;
#else
			u32 ID	= data->r_u32();
			children[i]	= ::Render->getVisual(ID);
#endif
		}
		bDontDelete = TRUE;
	}
	else
	{	
    	if (data->find_chunk(OGF_CHILDREN))
		{
			// From stream
            IReader* OBJ = data->open_chunk(OGF_CHILDREN);
            if (OBJ){
                IReader* O = OBJ->open_chunk(0);
                for (int count=1; O; count++) {
					string_path			name_load,short_name,num;
					strcpy_s				(short_name,N);
					if (strext(short_name)) *strext(short_name)=0;
					strconcat			(sizeof(name_load),name_load,short_name,":",itoa(count,num,10));
					children.push_back	(::Render->model_CreateChild(name_load,O));
                    O->close			();
                    O = OBJ->open_chunk	(count);
                }
                OBJ->close();
            }
			bDontDelete = FALSE;
        }
		else
		{
			FATAL		("Invalid visual");
    	}
	}
	float elps = perf.GetElapsed_sec() * 1000.f;
	if (elps > 15.f)
		MsgCB("##PERF_WARN: FHierrarhyVisual load from %s elapsed time = %.1f ms", N, elps);

	
	static u64 last_loaded = 0;
	total_loaded += (u64)chsz;
	if (total_loaded - last_loaded > 8 * 1048576)
	{
		MsgCB("##PERF_MEM: total loaded in FHierrarhyVisual::Load = %.1f MiB", size_in_mib(total_loaded));
		last_loaded = total_loaded;
	}
}

void	FHierrarhyVisual::Copy(IRender_Visual *pSrc)
{
	IRender_Visual::Copy	(pSrc);

	FHierrarhyVisual	*pFrom = (FHierrarhyVisual *)pSrc;

	children.clear	();
	children.reserve(pFrom->children.size());
	for (u32 i=0; i<pFrom->children.size(); i++) {
		IRender_Visual *p = ::Render->model_Duplicate	(pFrom->children[i]);
		children.push_back(p);
	}
	bDontDelete = FALSE;
}
