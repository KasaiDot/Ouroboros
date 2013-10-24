#ifndef OUROBOROS_H
#define OUROBOROS_H

#include <QMainWindow>

#include "threadmanager.h"
#include "animedatabase.h"
#include "apimanager.h"

namespace Ui {
class Ouroboros;
}

class Ouroboros : public QMainWindow
{
    Q_OBJECT

public:
    explicit Ouroboros(QWidget *parent = 0);
    ~Ouroboros();

private:
    Ui::Ouroboros *ui;
};

#endif // OUROBOROS_H
