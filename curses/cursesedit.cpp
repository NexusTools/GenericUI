#include "cursesmainwindow.h"
#include "cursesbuttonbox.h"
#include "curseslineedit.h"

char keyToChar(char off, Qt::Key start, Qt::Key key) {
    return off + (key - start);
}

void CursesLineEdit::updateCursor() {
    CursesMainWindow::current()->setCursor(screenX() + qMin(text().length(), width()-1), screenY());
}

bool CursesLineEdit::processEvent(QEvent *e) {
    switch(e->type()) {

        case GUIEvent::GUIKeyTyped:
        {
            GUIKeyEvent* kEv = (GUIKeyEvent*)e;
            if(kEv->mod().testFlag(Qt::AltModifier))
                break;

            char ch = 0;
            if(kEv->key() >= Qt::Key_A && kEv->key() <= Qt::Key_Z)
                ch = keyToChar(kEv->mod().testFlag(Qt::ShiftModifier) ? 'A' : 'a',
                               Qt::Key_A, kEv->key());
            else if(kEv->key() >= Qt::Key_0 && kEv->key() <= Qt::Key_1)
                ch = keyToChar('0', Qt::Key_0, kEv->key());
            else
                switch(kEv->key()) {
                    case Qt::Key_Space:
                        ch = ' ';
                        break;

                    case Qt::Key_Bar:
                        ch = '-';
                        break;

                    case Qt::Key_Period:
                        ch = '.';
                        break;

                    case Qt::Key_Exclam:
                        ch = '!';
                        break;

                    case Qt::Key_Slash:
                        ch = '/';
                        break;

                    case Qt::Key_Backslash:
                        ch = '\\';
                        break;

                    case Qt::Key_Backspace:
                        setText(text().left(text().size()-1));
                        updateCursor();
                        return true;

                    default:
                        break;
                }

            if(ch) {
                setText(text() += ch);
                updateCursor();
                return true;
            }
            break;
        }

        case GUIEvent::GUIFocusChanged:
            markDirty();
            break;

        case GUIEvent::GUIScreenPositionChanged:
            if(!isFocused())
                break;
        case GUIEvent::GUIFocusGained:
            updateCursor();
            break;

        case GUIEvent::GUIFocusLost:
            CursesMainWindow::current()->setCursor(-1, -1);
            break;

        default:
            break;
    }

    return CursesBase::processEvent(e);
}
