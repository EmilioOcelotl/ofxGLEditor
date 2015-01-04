#pragma once

#include "ofMain.h"
#include "ofxEditorColorScheme.h"
#include "ofxEditorSettings.h"

/// full screen text editor with optional syntax highlighting,
/// based on the Fluxus GLEditor
///
/// TODO: support UTF-8 characters
///
class ofxEditor {

	public:
	
		ofxEditor();
		ofxEditor(ofxEditorSettings &sharedSettings); //< share settings object
		virtual ~ofxEditor();
		
	/// \section Static Settings
		
		/// load font to be used by all editors, *must* be a fixed width font
		///
		/// call this before drawing any editor
		///
		static bool loadFont(const string & font, int size);
	
		/// set useSuper = true if you want to use the Super (Windows key, Mac CMD)
		/// key as the modifier key, otherwise false uses CTRL key
		/// default: true on Mac & false on all other platforms
		static void setSuperAsModifier(bool useSuper);
		static bool getSuperAsModifier();
	
	/// \section Main
		
		/// draw the editor, pushes view and applies viewport
		virtual void draw();
	
		/// draw the text field character grid
		virtual void drawGrid();
		
		/// required for interactive editing, etc
		virtual void keyPressed(int key);
		
		/// set the view port size,
		/// size of text field calculated based on font size
		virtual void resize();
		virtual void resize(int width, int height);
	
		/// get text buffer contents or current selection
		virtual string getText();
	
		/// set text buffer contents
		virtual void setText(const string& text);
	
		// insert text at the current buffer position
		virtual void insertText(const string& text);
	
		/// clear text buffer contents
		virtual void clearText();
	
	/// \section Settings

		/// access to the internal settings object
		///
		/// example usage:
		///
		///   editor.getSettings().alpha = 0.5;
		///   editor.getSettings().tabWidth = 8;
		///   ... etc
		///
		ofxEditorSettings& getSettings();
	
	/// \section Color Scheme
	
		/// set color scheme for this editor and highlight syntax
		/// note: pointer is never deleted
		void setColorScheme(ofxEditorColorScheme *colorScheme);
	
		/// clear the current color scheme
		void clearColorScheme();
	
		/// get the current color scheme, returns NULL if not set
		ofxEditorColorScheme* getColorScheme();
	
	/// \section Display Settings
	
		/// enable/disable line wrapping
		void setLineWrapping(bool wrap=true);
	
		/// get line wrapping value
		bool getLineWrapping();
	
		/// enable/disable line numbers
		void setLineNumbers(bool numbers=true);
	
		/// get line numbers value
		bool getLineNumbers();
	
	/// \section Current Position & Info
	
		/// animate the cursor so it's easy to find
		void blowupCursor();
	
		/// get the total number of lines in the text buffer
		unsigned int getNumLines();
	
		/// get the total number of characters in the text buffer
		unsigned int getNumCharacters();
	
		/// get the 1D cursor position in the text buffer
		unsigned int getCurrentPos();
	
		/// set the 1D cursor position in the text buffer
		void setCurrentPos(unsigned int pos);
	
		/// get the line the cursor is on
		unsigned int getCurrentLine();
	
		/// set the current line for the cursor
		void setCurrentLine(unsigned int line);
	
		/// get the current line character position of the cursor
		unsigned int getCurrentLinePos();
	
		/// get length of the current line cursor is on
		unsigned int getCurrentLineLen();
	
		/// get the cursor position by line and line character
		void setCurrentLinePos(unsigned int line, unsigned int character);
	
		/// reset position & selection
		void reset();
	
	/// \section Drawing Utils
	
		/// draw a string using the current editor font
		void drawString(const string& s, float x, float y);
		void drawString(const string& s, ofPoint& p);
	
		/// get the fixed width of a char using the current editor font
		int getCharWidth();
	
		/// get the fixed height of a char using the current editor font
		int getCharHeight();
	
	protected:
	
		/// custom font class to get access to protected functions
		class Font : public ofTrueTypeFont {
			public:
				void drawCharacter(int c, float x, float y) {
					drawCharAsShape(c, x, y);
				}
		};
	
	/// \section Static Variables
	
		static ofPtr<Font> s_font;       //< global editor font
		static int s_charWidth;          //< char block pixel width
		static int s_charHeight;         //< char block pixel height
		static int s_cursorWidth;		 //< cursor width, 1/3 char width
	
		static bool s_superAsModifier;   //< use the super key as modifier?
	
		/// shared copy/paste buffer if system clipboard isn't available
		static string s_copyBuffer;
	
	/// \section Member Variables

		ofxEditorSettings *m_settings; //< editor settings object
		bool m_sharedSettings; //< are the settings shared? if so, do not delete
	
		string m_text; //< text buffer
		unsigned int m_numLines; //< number of lines in the text buffer
		
		ofRectangle m_viewport;     //< viewport when drawing editor
		unsigned int m_position;    //< 1D text pos within buffer
		unsigned int m_desiredXPos; //< desired char pos on current line
		
		int m_visibleChars; //< computed text field char width
		int m_visibleLines; //< computed text field num lines
	
		int m_matchingCharsHighlight[2]; //< start & end pos for matching chars highlight
	
		bool m_selection; //< is text being selected (shift+arrows)
		unsigned int m_highlightStart; //< highlight start pos in buffer
		unsigned int m_highlightEnd;   //< highlight end pos in buffer
	
		unsigned int m_leftTextPosition;   //< left start char pos for horz scrolling
		unsigned int m_topTextPosition;    //< top start pos in buffer for vert scrolling
		unsigned int m_bottomTextPosition; //< bottom end pos in buffer for vert scrolling
		unsigned int m_displayedLineCount; //< current number of displayed lines (may be diff from m_visibleLines)
	
		bool m_shiftState; //< is shift pressed?
		
		ofxEditorColorScheme *m_colorScheme; //< optional syntax color scheme
		bool m_lineWrapping; //< enable line wrapping in this editor?
		bool m_lineNumbers;  //< enable line numbers?
		unsigned int m_lineNumWidth; //< line number block width in chars 
		
		float m_time;         //< timestamp for calculating animations
		float m_delta;        //< difference from last timestamp
		float m_flash;        //< cursor flash animation time
		bool m_blowupCursor;  //< blow up the cursor?
		float m_blowup;       //< how much the cursor is being blown up
		
	/// \section Syntax Parser Types
		
		/// syntax parser TextBlock types
		enum TextBlockType {
			UNKNOWN,
			WORD,
			STRING,
			NUMBER,
			SPACE,
			TAB,
			ENDLINE,
			COMMENT_BEGIN, // tag only, no text
			COMMENT_END    // tag only, no text
		};
		
		/// syntax parser custom class to represent a contextual block of text
		class TextBlock {
			public:
				
				TextBlockType type; ///< block type
				string text; ///< block text string
				
				TextBlock() {
					clear();
				}
				
				TextBlock(const TextBlock &from) {
					type = from.type;
					text = from.text;
				}
				
				void clear() {
					type = UNKNOWN;
					text = "";
				}
		};
		list<TextBlock> m_textBlocks; //< syntax parser text block linked list
	
	/// \section Helper Functions
	
		// draw a matching char hilight char block rectanlge at pos
		void drawMatchingCharBlock(int x, int y);
	
		/// draw a selection char block rectangle at pos
		void drawSelectionCharBlock(int x, int y);
	
		/// draw the cursor at pos
		void drawCursor(int x, int y);
	
		/// draw current line number starting at a given pos, padded by digit width of last line number
		void drawLineNumber(int &x, int &y, int &currentLine);
	
		/// replace tabs in buffer with spaces
		void processTabs();
	
		/// get offset in buffer to the current line
		int offsetToCurrentLineStart();
	
		/// get the length of the next line from the current buffer pos
		int nextLineLength(int pos);
	
		/// get the length of the previous line from the current buffer pos
		int previousLineLength(int pos);
	
		/// get the length of the curent line from the current buffer pos
		int lineLength(int pos);
	
		/// get the start of the current line from the current buffer pos
		unsigned int lineStart(int pos);
	
		/// get the end of the current line from the current buffer pos
		unsigned int lineEnd(int pos);
	
		/// find matching open/close char highlight positions base on current buffer pos
		void parseMatchingChars();
	
		/// look forward for a close char
		void parseOpenChars(int pos, int type);
	
		/// look backward for an open char
		void parseCloseChars(int pos, int type);
	
		/// get the number of lines at a buffer pos
		unsigned int lineNumberForPos(unsigned int pos);
	
		/// copy selected text to the system clipboard or copy buffer
		/// note: clipboard only supported when using a GLFW Window
		void copySelection();
	
		/// paste text from the system clipboard or copy buffer
		/// note: clipboard only supported when using a GLFW Window
		void pasteSelection();
		
	private:
	
		/// text buffer changed, so update syntax text blocks and/or other info
		///
		/// TODO: replace this brute force method with a link list pointer based
		/// approach that updates only those text blocks around the current
		/// editor area
		void textBufferUpdated();
	
		/// parses text into text blocks
		void parseTextBlocks();
		
		/// clears current text block list
		void clearTextBlocks();
};
