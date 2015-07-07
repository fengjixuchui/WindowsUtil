#include "Export.h"
namespace PE
{
	namespace Export
	{
		PDWORD GetProcExportFuncTableAddress(PeDecoder& pe, LPCSTR lpProcName)
		{
			if (!pe.IsPE())
			{
				return NULL;
			}
			// ��������������, ����
			auto er = ExportReader(pe);
			PDWORD nameTable = er.NameTable();
			DWORD right = *er.NumberOfNames();
			DWORD left = 0;
			DWORD mid;
			int cmpResult;
			while (left <= right)
			{
				mid = (left + right) >> 1;
				cmpResult = strcmp(lpProcName, (LPCSTR)pe.GetRvaData(nameTable[mid]));
				if (!cmpResult)
				{
					// �ҵ�
					auto nameOrdinal = er.OrdinalTable()[mid];
					return &er.FuncTable()[nameOrdinal];
				}
				if (cmpResult < 0)
				{
					right = mid - 1;
				}
				else
				{
					left = mid + 1;
				}
			}
			return NULL;
		}
		FARPROC GetProcAddress(PeDecoder& pe, LPCSTR lpProcName)
		{
			auto result = GetProcExportFuncTableAddress(pe, lpProcName);
			if (!result)
			{
				return NULL;
			}
			return (FARPROC)pe.GetRvaData(*result);
		}
		FARPROC GetProcAddress(HMODULE module, LPCSTR lpProcName)
		{
			PeDecoder pe(module, true);
			return GetProcAddress(pe, lpProcName);
		}

		FARPROC GetProcAddress(PeDecoder& pe, PVOID compareName, bool compareCallback(PVOID compare, LPCSTR procName))
		{
			if (!pe.IsPE())
			{
				return NULL;
			}
			if (compareCallback == NULL)
			{
				return NULL;
			}
			ExportReader er(pe);
			while (er.Next())
			{
				auto nameRva = er.CurrentNameRva();
				if (nameRva)
				{
					auto name = (PCHAR)pe.GetRvaData(*nameRva);
					if (compareCallback(compareName, name))
					{
						return (FARPROC)pe.GetRvaData(*er.CurrentFuncRva());
					}
				}
			}
		}
		FARPROC GetProcAddress(HMODULE module, PVOID compareName, bool compareCallback(PVOID compare, LPCSTR procName))
		{
			PeDecoder pe(module, true);
			return GetProcAddress(pe, compareName, compareCallback);
		}
	}
}