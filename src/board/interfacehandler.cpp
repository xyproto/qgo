/***************************************************************************
 *   Copyright (C) 2009 by The qGo Project                                 *
 *                                                                         *
 *   This file is part of qGo.   					   *
 *                                                                         *
 *   qGo is free software: you can redistribute it and/or modify           *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 *   or write to the Free Software Foundation, Inc.,                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


/***************************************************************************
 *
 * interfacehandler.cpp
 * This class deals with the tree information taken from the tree when 
 * navigating in it.
 * It is called by the BoardHandler (which does the actual navigation), and 
 * sets the interface widgets accodrding to the moves information
 *
 ***************************************************************************/


#include "interfacehandler.h"
#include "boardwindow.h"
#include "gamedata.h"

/*
struct ButtonState
{
    bool navPrevVar, navNextVar, navBackward, navForward, navFirst, navStartVar, navMainBranch,
		navLast, navNextBranch, navPrevComment, navNextComment, navIntersection, editPaste, editPasteBrother; // SL added eb 11
};
*/

InterfaceHandler::InterfaceHandler(BoardWindow *bw)
{
	boardwindow = bw;
//    buttonState = new ButtonState;
	scored_flag = false;

	tabPlay  = boardwindow->getUi()->tabTools->widget(0); 
	tabEdit = boardwindow->getUi()->tabTools->widget(1) ;

	GameMode mode = boardwindow->getGameMode();


	if (mode == modeNormal || mode == modeComputer)
		boardwindow->getUi()->commentEdit2->setDisabled(true);

}

InterfaceHandler::~InterfaceHandler()
{
//    delete buttonState;
}

/*
 * Resets all displays on the board window
 */
void InterfaceHandler::clearData()
{
    // qDebug("void InterfaceHandler::clearData()");
	
	clearComment();
//	setMoveData(0, true, 0, 0, false, false, false);
//    modeButton->setOn(false);
//	  mainWidget->setToolsTabWidget(tabNormalScore);
//    mainWidget->editButtonGroup->setButton(0);
//    editTools->hide();
	boardwindow->capturesBlack->setText("0");
	boardwindow->capturesWhite->setText("0");
	
//	if (board->getGameMode() != modeObserve && 
//		board->getGameMode() != modeMatch &&
//		board->getGameMode() != modeTeach)
//	{
		boardwindow->pb_timeBlack->setText("00:00");
		boardwindow->pb_timeWhite->setText("00:00");
//	}
//    normalTools->show();
	boardwindow->scoreButton->setDown(false);
//	editPaste->setEnabled(false);
//	editPasteBrother->setEnabled(false);
	boardwindow->slider->setValue(0);
    boardwindow->slider->setMaximum(0);
	scored_flag = false;
}

/*
 * displays the informations relative to a game on the board window
 */
void InterfaceHandler::updateCaption(GameData *gd)
{
	// Print caption
	// example: qGo 0.0.5 - Zotan 8k vs. tgmouse 10k
	// or if game name is given: qGo 0.0.5 - Kogo's Joseki Dictionary
	boardwindow->setWindowTitle( /* QString(isModified ? "* " : "") + */
		( (gd->number != 0 && gd->number < 10000) ?
			"(" + QString::number(gd->number) + ") " : 
			QString()) + (gd->gameName.isEmpty() ?
				gd->white_name 
				+ (!gd->white_rank.isEmpty() ?
					" " + gd->white_rank : 
					QString()) 
				+ " " + QObject::tr("vs.") + " " + gd->black_name 
				+ (!gd->black_rank.isEmpty() ?
					" " + gd->black_rank : 
					QString()) :
				gd->gameName) +	"   " + QString(PACKAGE));


	bool simple = gd->white_rank.length() == 0 && gd->black_rank.length() == 0;
	QGroupBox *gb = boardwindow->getUi()->whiteFrame;
	/* This name stuff is super redundant with below FIXME */
	QString player = gd->white_name;
	if (simple && player == QObject::tr("White"))
		gb->setTitle(QObject::tr("White"));	
	else if(!gd->nigiriToBeSettled)
	{
		// truncate to 13 characters max
		player.truncate(13);

		if (gd->white_rank.length() != 0)
			player = QObject::tr("W") + ": " + player + " " + gd->white_rank;
		else
			player = QObject::tr("W") + ": " + player;
		
		gb->setTitle(player);
	}

	gb = boardwindow->getUi()->blackFrame;

	player = gd->black_name;
	if (simple && player == QObject::tr("Black"))
		gb->setTitle(QObject::tr("Black"));	
	else if(!gd->nigiriToBeSettled)
	{
		// truncate to 13 characters max
		player.truncate(13);

		if (gd->black_rank.length() != 0)
			player = QObject::tr("B") + ": " + player + " " + gd->black_rank;
		else
			player = QObject::tr("B") + ": " + player;
		
		gb->setTitle(player);
	}
	
	//TODO set  clock
	
	if(gd->free_rated == RATED)
        boardwindow->freeratedLabel->setText("Rated");
	else if(gd->free_rated == FREE)
        boardwindow->freeratedLabel->setText("Free");
	else if(gd->free_rated == TEACHING)
        boardwindow->freeratedLabel->setText("Teaching");
	
    boardwindow->komiLabel->setText(QString("Komi: ").append(QString().setNum(gd->komi)));
    boardwindow->handicapLabel->setText(QString("H: ").append(QString().setNum(gd->handicap)));
}

/*
 * displays the informations relative to a move on the board window
 */
void InterfaceHandler::setMoveData(int n, bool black, int brothers, int sons, bool hasParent, bool hasPrev, bool hasNext, int lastX, int lastY)
{
	// move number
	QString s(QObject::tr("Move") + " ");
	s.append(QString::number(n));
	
	// color and coordinates
	if (lastX >= 1 && lastX <= boardwindow->getBoardSize() && lastY >= 1 && lastY <= boardwindow->getBoardSize())
	{
		s.append(" (");
		s.append(black ? QObject::tr("W")+" " : QObject::tr("B")+" ");
		s.append(QString(QChar(static_cast<const char>('A' + (lastX<9?lastX:lastX+1) - 1))) +
			QString::number(boardwindow->getBoardSize()-lastY+1) + ")");
	}

	//pass move
	else if (lastX == 20 && lastY == 20)
	{
		s.append(" (");
		s.append(black ? QObject::tr("W")+" " : QObject::tr("B")+" ");
		s.append(" " + QObject::tr("Pass") + ")");
	}

    boardwindow->moveNumLabel->setText(s);
//	statusTurn->setText(" " + s.right(s.length() - 5) + " ");  // Without 'Move '
	
//	statusNav->setText(" " + QString::number(brothers) + "/" + QString::number(sons));

	// set turn information (and color on the edit button)
	s = black ? QObject::tr("Black to play") : QObject::tr("White to play");
	boardwindow->getUi()->turnLabel->setText(s);
	
	boardwindow->getUi()->colorButton->setIcon(black ? QIcon(":/boardicons/resources/pics/stone_black.png") : QIcon(":/boardicons/resources/pics/stone_white.png") );

	// sons and variatons display
	s = "";
	s.append(QString::number(brothers));
	if (brothers == 1)
		s.append(" " + QObject::tr("brother") + "\n");
	else
		s.append(" " + QObject::tr("brothers") + "\n");

	s.append(QString::number(sons));
	if (sons == 1)
		s.append(" " + QObject::tr("son"));
	else
		s.append(" " + QObject::tr("sons"));
	boardwindow->getUi()->varLabel->setText(s);
	
	if(boardwindow->getGameMode() == modeReview)
	{
		/* For now, just disable navigation if its in the review mode, they can always duplicate
		 * the board and a lot more is necessary for qgo to do reviews. */
		boardwindow->getUi()->navPrevVar->setEnabled(false);
		boardwindow->getUi()->navNextVar->setEnabled(false);
		boardwindow->getUi()->navBackward->setEnabled(false);
		boardwindow->getUi()->navForward->setEnabled(false);
		boardwindow->getUi()->navFirst->setEnabled(false);
		boardwindow->getUi()->navStartVar->setEnabled(false);
		boardwindow->getUi()->navMainBranch->setEnabled(false);
		boardwindow->getUi()->navLast->setEnabled(false);
		boardwindow->getUi()->navNextBranch->setEnabled(false);
		boardwindow->getUi()->swapVarButton->setEnabled(false);
		boardwindow->getUi()->navPrevComment->setEnabled(false);
		boardwindow->getUi()->navNextComment->setEnabled(false);
		boardwindow->getUi()->navIntersection->setEnabled(false);
		
		boardwindow->getUi()->slider->setEnabled(false);
	}
	else if (boardwindow->getGameMode() == modeNormal || boardwindow->getGameMode() == modeObserve )//|| board->getGameMode() == modeEdit)
	{
		// Update the toolbar buttons
		boardwindow->getUi()->navPrevVar->setEnabled(hasPrev);
		boardwindow->getUi()->navNextVar->setEnabled(hasNext);
		boardwindow->getUi()->navBackward->setEnabled(hasParent);
		boardwindow->getUi()->navForward->setEnabled(sons);
		boardwindow->getUi()->navFirst->setEnabled(hasParent);
		boardwindow->getUi()->navStartVar->setEnabled(hasParent);
		boardwindow->getUi()->navMainBranch->setEnabled(hasParent);
		boardwindow->getUi()->navLast->setEnabled(sons);
		boardwindow->getUi()->navNextBranch->setEnabled(sons);
		boardwindow->getUi()->swapVarButton->setEnabled(hasPrev);
		boardwindow->getUi()->navPrevComment->setEnabled(hasParent);
		boardwindow->getUi()->navNextComment->setEnabled(sons);
		boardwindow->getUi()->navIntersection->setEnabled(true);
		
		boardwindow->getUi()->slider->setEnabled(true);
	}
/*	else  if (board->getGameMode() == modeObserve)
	{
		// Update the toolbar buttons
		navBackward->setEnabled(hasParent);
		navForward->setEnabled(sons);
		navFirst->setEnabled(hasParent);
		navLast->setEnabled(sons);
		navPrevComment->setEnabled(hasParent);
		navNextComment->setEnabled(sons);
		navIntersection->setEnabled(true);

		slider->setEnabled(true);

//		board->getBoardHandler()->display_incoming_move = !bool(sons);

	}
	else
		slider->setDisabled(true);
*/	
	// Update slider
	boardwindow->getUi()->slider->blockSignals (true);

//	int mv = boardwindow->getUi()->slider->maximum();
//	int v = boardwindow->getUi()->slider->value();

	if (boardwindow->getUi()->slider->maximum() < n)
		  setSliderMax(n);

	// we need to be carefull with the slider :
	// normal case, slider is moved
//	if (board->getGameMode() != modeObserve ||
	// observing, but browsing (no incoming move)
//	(board->getGameMode() == modeObserve && mv >= n) ||
	// observing, but at the last move, and an incoming move occurs 
//	(board->getGameMode() == modeObserve && mv < n && v==n-1))
		boardwindow->getUi()->slider->setValue(n);

	boardwindow->getUi()->slider->blockSignals (false);
}


/*
 * display text in the comment area
 */
void InterfaceHandler::displayComment(const QString &c)
{
//	if (board->get_isLocalGame())
//	{
		if (c.isEmpty())
			boardwindow->getUi()->commentEdit->clear();
		else
			boardwindow->getUi()->commentEdit->setText(c);
//	}
//	else if (!c.isEmpty())
//			commentEdit->append(c);
}


/*
 * clear the big field (offline)
 */
void InterfaceHandler::clearComment()
{
	boardwindow->commentEdit->clear();
}


/*
 * modifies the maximum value of the slider (used when a move is added)
 */
void InterfaceHandler::setSliderMax(int n)
{
	if (n < 0)
		n = 0;

    if (n == boardwindow->getUi()->slider->maximum())
        return;

	boardwindow->getUi()->slider->setMaximum(n);
    boardwindow->getUi()->sliderRightLabel->setText(QString::number(n));
}

/*
 * Sets the number of prisonners on the UI
 */
void InterfaceHandler::setCaptures(float black, float white)
{

	boardwindow->getUi()->capturesBlack->setText(QString::number(black));
	boardwindow->getUi()->capturesWhite->setText(QString::number(white));
}

/*
 * This updates the UI with the correct layout depending on the game mode
 */
/* FIXME this overlaps with stuff in boardhandler I think it is, or boardwindow */
void InterfaceHandler::toggleMode(GameMode mode)
{
	
	switch (mode)
	{
		
	case modeNormal:
//		modeButton->setEnabled(true);
//		mainWidget->setToolsTabWidget(tabEdit, tabEnable);
		boardwindow->getUi()->actionPlay->setEnabled(true);
		boardwindow->getUi()->tabTools->setCurrentIndex(0) ;//setVisible(false);
		boardwindow->getUi()->scoreButton->setEnabled(true);
//		scoreButton->setText(QObject::tr("Score", "button label"));
		boardwindow->getUi()->passButton_2->setEnabled(true);
//		boardwindow->getUi()->refreshButton_2->setDisabled(true);
//		undoButton->setDisabled(true);
//		resignButton->setDisabled(true);
//		adjournButton->setDisabled(true);
//		refreshButton->setDisabled(true);
		boardwindow->getUi()->commentEdit->setReadOnly(false);
		boardwindow->getUi()->commentEdit2->setDisabled(true);
//		statusMode->setText(" " + QObject::tr("E", "Board status line: edit mode") + " ");
//		statusMark->setText(getStatusMarkText(board->getMarkType()));
		return;
		
	case modeObserve:
//		modeButton->setDisabled(true);
//		mainWidget->setToolsTabWidget(tabEdit, tabDisable);
//		boardwindow->getUi()->tabTools->removeTab(0) ;
		boardwindow->getUi()->actionPlay->setDisabled(true);
		boardwindow->getUi()->tabTools->setVisible(false) ;
//		boardwindow->getUi()->toolFrame->layout()->addItem(new QSpacerItem()) ;
//		boardwindow->getUi()->scoreButton_2->setDisabled(true);
//		scoreButton->setText(QObject::tr("Edit", "button label"));
		boardwindow->getUi()->passButton_2->setDisabled(true);
//		boardwindow->getUi()->undoButton->setDisabled(true);
//		boardwindow->getUi()->resignButton->setDisabled(true);
//		boardwindow->getUi()->adjournButton->setDisabled(true);
//		boardwindow->getUi()->refreshButton_2->setEnabled(true);
		boardwindow->getUi()->commentEdit->setReadOnly(true);
		boardwindow->getUi()->commentEdit2->setReadOnly(false);
		boardwindow->getUi()->commentEdit2->setEnabled(true);
//		editCut->setEnabled(false);
//		editDelete->setEnabled(false);
//		fileNew->setEnabled(false);
//		fileNewBoard->setEnabled(false);
//		fileOpen->setEnabled(false);
//		statusMode->setText(" " + QObject::tr("O", "Board status line: observe mode") + " ");
//		statusMark->setText(getStatusMarkText(board->getMarkType()));
		return ;
		
	case modeMatch : 
//		modeButton->setDisabled(true);
//		mainWidget->setToolsTabWidget(tabEdit, tabDisable);
		boardwindow->getUi()->actionPlay->setDisabled(true);
		boardwindow->getUi()->tabTools->setCurrentIndex(1) ;
		boardwindow->getUi()->scoreButton->setDisabled(true);
//		scoreButton->setText(QObject::tr("Edit", "button label"));
		boardwindow->getUi()->passButton->setEnabled(true);
//		passButton->setText(QObject::tr("Pass", "button label"));
		boardwindow->getUi()->undoButton->setEnabled(true);
		boardwindow->getUi()->resignButton->setEnabled(true);
		boardwindow->getUi()->reviewButton->setDisabled(true);
		boardwindow->getUi()->reviewButton->setVisible(true);
		/*boardwindow->getUi()->reviewButton->setText(QApplication::translate("BoardWindow", "Request Count", 0, QApplication::UnicodeUTF8));
		if(boardwindow->getBoardDispatch()->supportsRequestCount())
			boardwindow->getUi()->reviewButton->setEnabled(true);
		else
			boardwindow->getUi()->reviewButton->setEnabled(false);*/
		boardwindow->getUi()->adjournButton->setEnabled(true);
		/* FIXME we could have the refreshButton refresh the observers on
		 * IGS.  This requires a "supports" protocol function, etc., to
		 * send the refresh.  Or we could just periodically refresh the
		 * observers.
		 * But note that its currently not "connect"ed to anything. */
		//boardwindow->getUi()->refreshButton->setEnabled(true);
		boardwindow->getUi()->refreshButton->setEnabled(false);
		
		boardwindow->getUi()->doneButton->setEnabled(false);
		boardwindow->getUi()->commentEdit->setReadOnly(true);
		boardwindow->getUi()->commentEdit2->setEnabled(true);
		boardwindow->getUi()->commentEdit2->setReadOnly(false);
		boardwindow->getUi()->navButtonsFrame->setEnabled(false);
//		commentEdit2->setReadOnly(false);
//		commentEdit2->setDisabled(false);
//		fileNew->setEnabled(false);
//		fileNewBoard->setEnabled(false);
//		fileOpen->setEnabled(false);
//		statusMode->setText(" " + QObject::tr("P", "Board status line: play mode") + " ");
//		statusMark->setText(getStatusMarkText(board->getMarkType()));
		return ;

	case   modeComputer :
//		modeButton->setDisabled(true);
//		mainWidget->setToolsTabWidget(tabEdit, tabDisable);
		boardwindow->getUi()->actionPlay->setDisabled(true);
		boardwindow->getUi()->tabTools->setCurrentIndex(1) ;
		boardwindow->getUi()->scoreButton->setDisabled(true);
//		scoreButton->setText(QObject::tr("Edit", "button label"));
		boardwindow->getUi()->passButton->setEnabled(true);
//		passButton->setText(QObject::tr("Pass", "button label"));
		boardwindow->getUi()->undoButton->setEnabled(true);
		boardwindow->getUi()->resignButton->setEnabled(true);
		boardwindow->getUi()->adjournButton->setEnabled(false);
		//boardwindow->getUi()->refreshButton->setEnabled(false);
		boardwindow->getUi()->doneButton->setEnabled(false);
		boardwindow->getUi()->commentEdit->setReadOnly(true);
		boardwindow->getUi()->navButtonsFrame->setEnabled(false);
		boardwindow->getUi()->commentEdit2->setDisabled(true);
		boardwindow->getUi()->reviewButton->setDisabled(true);
//		fileNew->setEnabled(false);
//		fileNewBoard->setEnabled(false);
//		fileOpen->setEnabled(false);
//		statusMode->setText(" " + QObject::tr("P", "Board status line: play mode") + " ");
//		statusMark->setText(getStatusMarkText(board->getMarkType()));
		return ;
      		
	case modeTeach:
//		board->setMode(modeTeach);
//		modeButton->setDisabled(true);
		boardwindow->getUi()->actionPlay->setDisabled(true);
		boardwindow->getUi()->tabTools->setCurrentIndex(1) ;
//		mainWidget->setToolsTabWidget(tabEdit, tabDisable);
		boardwindow->getUi()->scoreButton->setDisabled(true);
//		scoreButton->setText(QObject::tr("Edit", "button label"));
		boardwindow->getUi()->passButton->setEnabled(true);
//		passButton->setText(QObject::tr("Pass", "button label"));
		boardwindow->getUi()->undoButton->setEnabled(true);
		boardwindow->getUi()->resignButton->setEnabled(true);
		boardwindow->getUi()->adjournButton->setEnabled(true);
		//boardwindow->getUi()->refreshButton->setEnabled(true);
		boardwindow->getUi()->doneButton->setEnabled(false);
		boardwindow->getUi()->commentEdit->setReadOnly(true);
		boardwindow->getUi()->navButtonsFrame->setEnabled(false);
		boardwindow->getUi()->commentEdit2->setReadOnly(false);
		boardwindow->getUi()->commentEdit2->setEnabled(true);
//		commentEdit2->setReadOnly(false);
//		commentEdit2->setDisabled(false);
//		fileNew->setEnabled(false);
//		fileNewBoard->setEnabled(false);
//		fileOpen->setEnabled(false);
//		statusMode->setText(" " + QObject::tr("T", "Board status line: teach mode") + " ");
//		statusMark->setText(getStatusMarkText(board->getMarkType()));
		return ;
	
	case modeReview :
//		board->setMode(modeTeach);
//		modeButton->setDisabled(true);
		boardwindow->getUi()->actionPlay->setDisabled(true);
		boardwindow->getUi()->tabTools->setCurrentIndex(1) ;
//		mainWidget->setToolsTabWidget(tabEdit, tabDisable);
		boardwindow->getUi()->scoreButton->setDisabled(true);
//		scoreButton->setText(QObject::tr("Edit", "button label"));
		boardwindow->getUi()->passButton->setEnabled(true);
//		passButton->setText(QObject::tr("Pass", "button label"));
		boardwindow->getUi()->undoButton->setEnabled(true);
		boardwindow->getUi()->resignButton->setEnabled(true);
		boardwindow->getUi()->adjournButton->setEnabled(true);
		//boardwindow->getUi()->refreshButton->setEnabled(true);
		boardwindow->getUi()->doneButton->setEnabled(false);
		boardwindow->getUi()->commentEdit->setReadOnly(true);
		boardwindow->getUi()->commentEdit2->setReadOnly(false);
		boardwindow->getUi()->commentEdit2->setEnabled(true);
//		fileNew->setEnabled(false);
//		fileNewBoard->setEnabled(false);
//		fileOpen->setEnabled(false);
//		statusMode->setText(" " + QObject::tr("T", "Board status line: teach mode") + " ");
//		statusMark->setText(getStatusMarkText(board->getMarkType()));
		return ;
	case modeUndefined:
		return;	
//	default:
//		return modeNormal;
	}
}

/*
void InterfaceHandler::toggleToolbarButtons(bool state)
{
//    CHECK_PTR(buttonState);
	
//    buttonState->navPrevVar = navPrevVar->isEnabled();
	boardwindow->getUi()->navPrevVar->setEnabled(state);
	
//    buttonState->navNextVar = navNextVar->isEnabled();
	boardwindow->getUi()->navNextVar->setEnabled(state);
	
//    buttonState->navBackward = navBackward->isEnabled();
	boardwindow->getUi()->navBackward->setEnabled(state);
    
//    buttonState->navForward = navForward->isEnabled();
	boardwindow->getUi()->navForward->setEnabled(state);
	
//    buttonState->navFirst = navFirst->isEnabled();
	boardwindow->getUi()->navFirst->setEnabled(state);
	
//    buttonState->navStartVar = navStartVar->isEnabled();
	boardwindow->getUi()->navStartVar->setEnabled(state);
	
//    buttonState->navMainBranch = navMainBranch->isEnabled();
	boardwindow->getUi()->navMainBranch->setEnabled(state);
	
//    buttonState->navLast = navLast->isEnabled();
	boardwindow->getUi()->navLast->setEnabled(state);
	
    buttonState->navNextBranch = navNextBranch->isEnabled();
    navNextBranch->setEnabled(false);
	
    buttonState->navPrevComment = navPrevComment->isEnabled();
	  navPrevComment->setEnabled(false);

    buttonState->navNextComment = navNextComment->isEnabled();
	  navNextComment->setEnabled(false);

    buttonState->navIntersection = navIntersection->isEnabled(); // added eb 111
	  navIntersection->setEnabled(false);                          // end add eb 11

    buttonState->editPaste = editPaste->isEnabled();
    editPaste->setEnabled(false);
	
    buttonState->editPasteBrother = editPasteBrother->isEnabled();
    editPasteBrother->setEnabled(false);
    
    navNthMove->setEnabled(false);
    navAutoplay->setEnabled(false);
    editCut->setEnabled(false);
    editDelete->setEnabled(false);
    navEmptyBranch->setEnabled(false);
    navCloneNode->setEnabled(false);
    navSwapVariations->setEnabled(false);
    fileImportASCII->setEnabled(false);
    fileImportASCIIClipB->setEnabled(false);
    fileImportSgfClipB->setEnabled(false);

}
*/

/*
 * displays the score in the score layout
 */
void InterfaceHandler::setScore(int terrB, int capB, int terrW, int capW, float komi)
{
	GameData * gd = boardwindow->getGameData();
	bool simple = gd->white_rank.length() == 0 && gd->black_rank.length() == 0;
	QGroupBox *gb = boardwindow->getUi()->whiteFrame_score;

	QString player = gd->white_name;
	if (simple && player == QObject::tr("White"))
		gb->setTitle(QObject::tr("White"));	
	else
	{
		// truncate to 13 characters max
		player.truncate(13);

		if (gd->white_rank.length() != 0)
			player = QObject::tr("W") + ": " + player + " " + gd->white_rank;
		else
			player = QObject::tr("W") + ": " + player;
		
		gb->setTitle(player);
	}

	gb = boardwindow->getUi()->blackFrame_score;

	player = gd->black_name;
	if (simple && player == QObject::tr("Black"))
		gb->setTitle(QObject::tr("Black"));	
	else
	{
		// truncate to 13 characters max
		player.truncate(13);

		if (gd->black_rank.length() != 0)
			player = QObject::tr("B") + ": " + player + " " + gd->black_rank;
		else
			player = QObject::tr("B") + ": " + player;
		
		gb->setTitle(player);
	}

	boardwindow->getUi()->komiScore->setText(QString::number(komi));
	boardwindow->getUi()->terrWhite->setText(QString::number(terrW));
	boardwindow->getUi()->capturesWhiteScore->setText(QString::number(capW));
	boardwindow->getUi()->totalWhite->setText(QString::number((float)terrW + (float)capW + komi));
	boardwindow->getUi()->terrBlack->setText(QString::number(terrB));
	boardwindow->getUi()->capturesBlackScore->setText(QString::number(capB));
	boardwindow->getUi()->totalBlack->setText(QString::number(terrB + capB));
}
