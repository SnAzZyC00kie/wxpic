#ifndef SESSIONDIALOG_H
#define SESSIONDIALOG_H

//(*Headers(TSessionDialog)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

#include "SessionManagerInt.h"

class TSessionDialog: public wxDialog
{
	public:


		/**/TSessionDialog(TSessionManager &pSessionManager, wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~TSessionDialog();

		//(*Declarations(TSessionDialog)
		wxListBox* aSessionListBox;
		wxStaticText* StaticText2;
		wxButton* aCloseButton;
		wxButton* aStartButton;
		wxButton* aStartCloseButton;
		wxButton* Button1;
		wxButton* aNewButton;
		wxTextCtrl* aSessionNameEdit;
		wxButton* aRefreshButton;
		wxButton* aRenameButton;
		wxButton* aSaveButton;
		wxButton* aDeleteButton;
		//*)

		// static T_CONFIG *GetSession (T_CONFIG *pConfig);

	protected:

		//(*Identifiers(TSessionDialog)
		static const long ID_SESSION_LISTBOX;
		static const long ID_NEW_BUTTON;
		static const long ID_START_BUTTON;
		static const long ID_START_CLOSE_BUTTON;
		static const long ID_DELETE_BUTTON;
		static const long ID_REFRESH_BUTTON;
		static const long ID_BUTTON1;
		static const long ID_CLOSE_BUTTON;
		static const long ID_STATICTEXT2;
		static const long ID_SESSION_NAME_EDIT;
		static const long ID_RENAME_BUTTON;
		static const long ID_SAVE_BUTTON;
		//*)

	private:
        TSessionManager               &aSessionManager;  //-- The creator's session manager
        TSessionManager::TSessionInfo *aSessionInfoTab;  //-- The last session information
        int                            aSessionCount;    //-- The number of sessions
        int                            aCurrentSession;  //-- The index of the current session (-1 if no current session)
        int                            aCurrentSelection;//-- The item currently selected in the list
        bool                           aHasFreeSession;  //-- Indicate if there is still a free session
        bool                           aIsSaved;         //-- Indicate if the current session config is saved
        bool                           aIsNameChanged;   //-- The Current session name has been edited but not applied


        //-- Define the content of the control aSessionListBox
        void fillSessionList (void);
        //-- Set the Enable state for the buttons that depends on the selection
        void setButtonStatus (void);
        //-- Save the configuration of current session
        void saveConfig      (void);
        //-- Start a new session and return the session number (-1 if creation failed)
        int  startSession    (void);
        //-- Rename the session and clear name change indicator
        void renameSession   (void);
        //-- Get the index of the selected session in the session list (must exist)
        int  getSelSession   (void) const;
        //-- Ask for saving unsave config. Return false if user canceled
        bool askSaveConfig   (bool pIsCreate);
        //-- Ask for saving unsave config. Return false if user canceled
        bool askRename       (void);
        //-- Combine the 2 question on Session Change
        bool askAll          (bool pIsCreate) { return askSaveConfig(pIsCreate) && askRename(); }

		//(*Handlers(TSessionDialog)
		void onNewButtonClick(wxCommandEvent& event);
		void onStartButtonClick(wxCommandEvent& event);
		void onStartCloseButtonClick(wxCommandEvent& event);
		void onDeleteButtonClick(wxCommandEvent& event);
		void onRefreshButtonClick(wxCommandEvent& event);
		void onCloseButtonClick(wxCommandEvent& event);
		void OnClose(wxCloseEvent& event);
		void onSessionNameEditText(wxCommandEvent& event);
		void onRenameButtonClick(wxCommandEvent& event);
		void onSaveButtonClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif