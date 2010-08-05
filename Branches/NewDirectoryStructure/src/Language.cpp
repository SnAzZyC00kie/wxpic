///////////////////////////////////////////////////////////////////////////////
// Name:        Language.cpp
// Purpose:     Manage the Language selection for an application
// Author:      Philippe Chevrier
// Modified by:
// Created:     2009.03.01
// Copyright:   (c) 2009 Philippe Chevrier <pch@laposte.net>
// License:     GNU Lesser General Public License (LGPLv3)
///////////////////////////////////////////////////////////////////////////////

#include "Language.h"

#include <wx/stdpaths.h>
#include <wx/strconv.h>
#include <wx/file.h>

TLanguage *TLanguage::theSingleton = NULL;

/* static */
bool TLanguage::SetLangDefDir (const wxFileName &pLanguageDefFile)
{
    if (theSingleton != NULL)
        return false;
    theSingleton = new TLanguage(pLanguageDefFile);
    return (theSingleton != NULL) && (theSingleton->aLanguageCount > 0);
}


/* static */
void TLanguage::SetLanguage (wxString &pLanguageName)
{
    theSingleton->setLanguage(pLanguageName);
}

void TLanguage::setLanguage (wxString &pLanguageName)
{
    //-- Si un langage a d�j� �t� d�fini on ne fait rien
    if (aLanguageSet == wxLANGUAGE_UNKNOWN)
    {
        //-- Index invalide correspondant � pas de langage trouv�
        int Language = aLanguageCount;
        if (!pLanguageName.IsEmpty())
        {
            //-- Si un Langage est demand� on le recherche parmi les langages existants
            for (int i = 0; i < aLanguageCount; ++i)
            {
                Language = aLanguageTab[i];
                if (wxLocale::GetLanguageInfo(Language)->Description == pLanguageName)
                    //-- Le langage � l'index courant a bien le nom demand�
                    //-- On a trouv� le bon index
                    break;
            }
            //-- Si on a pas trouv� de language correspondant au nom donn�
            //-- On efface ce nom ce qui signifie pas de language trouv�
            if (Language == aLanguageCount)
                pLanguageName = wxEmptyString;
        }
        //-- Aucun langage n'a �t� demand� on va tester le langage par d�faut du syst�me
        else if (try2Languages (wxLANGUAGE_DEFAULT))
        {
            //-- Le langage par d�faut du syst�me est disponible
            //-- C'est lui qui sera d�fini
            Language = wxLANGUAGE_DEFAULT;
            pLanguageName = wxLocale::GetLanguageInfo(Language)->Description;
        }
        //-- Si le langage est valide on va le d�finir comme langage courant
        if (!pLanguageName.IsEmpty())
            doSetLanguage(Language);
    }
}


void TLanguage::doSetLanguage (int pLanguage)
{
    //-- M�moriser le langage courant
    aLanguageSet = pLanguage;
    //-- D�finit la locale avec ce langage (encodage et fichier de traduction)
    aLocale.Init(aLanguageSet, wxLOCALE_CONV_ENCODING);
    aLocale.AddCatalog(aLanguageDefFile.GetName());
}


/*static*/
bool TLanguage::SetHelp (const wxString &pHelpDefDir)
{
    //-- Construit le fichier de map par d�faut quand aucun fichier n'est sp�cifi�
    	wxString HelpDir = pHelpDefDir;
		if (HelpDir.IsEmpty()){
			wxFileName Temp;
			Temp.Assign(wxStandardPaths::Get().GetExecutablePath());
			Temp.AppendDir(HELP_DEFAULT_DIR_NAME);
			HelpDir = Temp.GetPath();
			}
		if( wxFile::Exists(HelpDir) ){ //Silences error if diretory doesn't exists.
			theSingleton->aIsHelpValid = theSingleton->aHelpController.Initialize(HelpDir);
			return true;
			}
	return false;
}


/* static */
wxArrayString TLanguage::GetLanguageNameList(void)
{
    wxArrayString Result;
    for (int i = 0; i < theSingleton->aLanguageCount; ++i)
    {
        const wxLanguageInfo *LangInfo = wxLocale::GetLanguageInfo(theSingleton->aLanguageTab[i]);
        Result.Add(LangInfo->Description);
    }
    return Result;
}


/**/ TLanguage::TLanguage(const wxFileName &pLanguageDefFile)
: aLanguageCount  (0)
, aLanguageSet    (wxLANGUAGE_UNKNOWN)
, aLanguageDefFile(pLanguageDefFile)
, aIsHelpValid    (false)
{
    //-- Construit le r�pertoire par d�faut quand aucun r�pertoire de traduction n'est sp�cifi�
    if (aLanguageDefFile.GetPath().IsEmpty())
    {
        aLanguageDefFile.Assign(wxStandardPaths::Get().GetExecutablePath());
        aLanguageDefFile.AppendDir(LANGUAGE_DEFAULT_DIR_NAME);
    }
    //-- Dans tous les cas l'extension des fichiers de traduction est .mo
    aLanguageDefFile.SetExt(_T("mo"));
    //-- Indique que les catalogues doivent �tre cherch�s dans le r�pertoire de traduction
    aLocale.AddCatalogLookupPathPrefix(aLanguageDefFile.GetPath());

    //-- Passer tous les langages en revue pour savoir s'ils ont leur fichier de traduction
    //-- Si c'est le cas le language est ajout� dans la liste des langages disponibles
    for(int i = 0; i < wxLANGUAGE_USER_DEFINED; ++i)
    {
        if (i == wxLANGUAGE_DEFAULT)
            continue;
        if (try2Languages(i))
            aLanguageTab[aLanguageCount++] = i;
    }
}


bool TLanguage::try2Languages (int pLanguage)
{
    bool                  Result = false;
    const wxLanguageInfo *LangInfo = wxLocale::GetLanguageInfo(pLanguage);

    //-- D'abord le langage doit �tre valide
    if (LangInfo != NULL)
    {
        //-- Il doit avoir un nom !
        wxString LangTag (LangInfo->CanonicalName);
        Result = !LangTag.IsEmpty();
        if (Result)
        {
            //-- Alors on peut essayer de trouver le fichier de traduction
            Result = tryLanguage(LangTag);
            if (!Result)
            {
                //-- Si on n'a pas trouv� le fichier de traduction
                //-- Et si la langue est une variante nationale
                //-- On peut v�rifier si le langage g�n�rique existe
                int SubLangPos = LangTag.Find(_T('_'));
                if (SubLangPos != wxNOT_FOUND)
                {
                    //-- Construction du nom du langage g�n�rique
                    LangTag.Truncate(SubLangPos);

                    //-- Le langage g�n�rique �tant g�n�ralement g�n�rique de plusieurs variante r�gionale
                    //-- on v�rifie si le g�n�rique n'a pas d�j� �t� v�rifi� pour une variante pr�c�dente
                    static wxString LastSubLanguage;
                    if (LangTag != LastSubLanguage)
                    {
                        //-- C'est la premi�re fois on peut v�rifier la langue g�n�rique
                        Result = tryLanguage(LangTag);
                        //-- M�moriser que ce langague g�n�rique a �t� test�
                        LastSubLanguage = LangTag;
                    }
                }
            }
        }
    }
    return Result;
}


bool TLanguage::tryLanguage (const wxString &pLanguage)
{
    aLanguageDefFile.AppendDir(pLanguage);
    bool Result = aLanguageDefFile.FileExists();
    aLanguageDefFile.RemoveLastDir();
    return Result;
}


wxString TLanguage::doGetFileText (const wxString &pFilename)
{
    wxFileName Filename;
    bool       Found = false;
    wxString   Result;
    wxFile     TextFile;
    char      *FileData;

    Filename = aLanguageDefFile;
    Filename.SetFullName(pFilename);
    if (aLanguageSet != wxLANGUAGE_UNKNOWN)
    {
        //-- R�cup�ration du nom du r�pertoire de la langue
        wxString LangTag (wxLocale::GetLanguageInfo(aLanguageSet)->CanonicalName);
        Filename.AppendDir(LangTag);
        Found = Filename.FileExists();
        if (!Found)
        {
            Filename.RemoveLastDir();
            int SubLangPos = LangTag.Find(_T('_'));
            if (SubLangPos != wxNOT_FOUND)
            {
                LangTag.Truncate(SubLangPos);
                Filename.AppendDir(LangTag);
                Found = Filename.FileExists();
                if (!Found)
                    Filename.RemoveLastDir();
            }
        }
    }
    if (!Found && Filename.FileExists())
        Found = true;
    if (Found)
    {
        TextFile.Open(Filename.GetFullPath());
        Found = TextFile.IsOpened();
    }
    wxFileOffset Length;
    if (Found)
    {
        Length   = TextFile.Length();
#ifdef _UNICODE
        FileData = (char*)malloc(Length+1);
        Found    = (FileData != NULL);
    }
    if (Found)
    {
        FileData[Length] = '\0';
        if (TextFile.Read(FileData, Length) == Length)
        {
            wchar_t *StringBuf = Result.GetWriteBuf(Length+1);
            if (StringBuf != NULL)
            {
                Length = wxConvLocal.ToWChar(StringBuf, Length+1, FileData);
                if ((int)Length > 0)
                    --Length;
                else
                    Length = 0;
                Result.UngetWriteBuf(Length);
            }
        }
        free(FileData);
#else
        wchar_t *StringBuf = Result.GetWriteBuf(Length+1);
        if (StringBuf != NULL)
        {
            if (TextFile.Read(StringBuf, Length) != Length)
                Length = 0;
            Result.UngetWriteBuf(Length);
        }
#endif
    }
    return Result;
}
