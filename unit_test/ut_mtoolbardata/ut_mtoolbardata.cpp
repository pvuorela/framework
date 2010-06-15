#include "ut_mtoolbardata.h"

#include <mtoolbardata.h>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QDebug>

namespace {
    QString Toolbar1 = "/toolbar1.xml";
    QString Toolbar2 = "/toolbar2.xml";
    QString Toolbar3 = "/toolbar2.xml";
}

void Ut_MToolbarData::initTestCase()
{
    char *argv[1] = { (char *) "ut_mimpluginloader" };
    int argc = 1;

    app = new QCoreApplication(argc, argv);

    Toolbar1 = QCoreApplication::applicationDirPath() + Toolbar1;
    QVERIFY2(QFile(Toolbar1).exists(), "toolbar1.xml does not exist");
    Toolbar2 = QCoreApplication::applicationDirPath() + Toolbar2;
    QVERIFY2(QFile(Toolbar2).exists(), "toolbar2.xml does not exist");
    Toolbar3 = QCoreApplication::applicationDirPath() + Toolbar3;
    QVERIFY2(QFile(Toolbar3).exists(), "toolbar3.xml does not exist");
}

void Ut_MToolbarData::cleanupTestCase()
{
    delete app;
}

void Ut_MToolbarData::init()
{
    subject = new MToolbarData;
}

void Ut_MToolbarData::cleanup()
{
    delete subject;
    subject = 0;
}

void Ut_MToolbarData::testLoadXML()
{
    qDebug() << "Try to load file" << Toolbar1;
    QVERIFY2(QFile(Toolbar1).exists(), "toolbar1.xml does not exist");
    bool ok;
    QSharedPointer<const MToolbarRow> row;
    QSharedPointer<MToolbarItem> item;

    ok = subject->loadNokiaToolbarXml(Toolbar1);
    QVERIFY2(ok, "toolbar1.xml was not loaded correctly");

    QVERIFY(subject->locked() == true);

    //test lanscape part
    QSharedPointer<const MToolbarLayout> landscape = subject->layout(M::Landscape);
    QVERIFY(!landscape.isNull());
    QCOMPARE(landscape->rows().count(), 1);
    row = landscape->rows().at(0);
    QCOMPARE(row->items().count(), 5);

    item = row->items().at(0);
    QVERIFY(!item.isNull());
    QCOMPARE(item->type(), MInputMethod::ItemButton);
    QCOMPARE(item->name(), QString("test1"));
    QCOMPARE(item->group(), QString("group1"));
    QCOMPARE(item->priority(), 0);
    QCOMPARE(int(item->showOn()), int(MInputMethod::VisibleAlways));
    QCOMPARE(int(item->hideOn()), int(MInputMethod::VisibleUndefined));
    QCOMPARE(item->alignment(), Qt::AlignRight);
    QCOMPARE(item->text(), QString("text1"));
    QCOMPARE(item->textId(), QString("id1"));
    QCOMPARE(item->isVisible(), true);
    QCOMPARE(item->toggle(), false);
    QCOMPARE(item->pressed(), false);
    QCOMPARE(item->icon(), QString("icon1"));
    QCOMPARE(item->size(), 100);

    QCOMPARE(item->actions().count(), 1);
    QCOMPARE(item->actions().at(0)->type(), MInputMethod::ActionSendString);
    QCOMPARE(item->actions().at(0)->keys(), QString());
    QCOMPARE(item->actions().at(0)->text(), QString("sendstring1"));
    QCOMPARE(item->actions().at(0)->command(), QString());
    QCOMPARE(item->actions().at(0)->group(), QString());

    item = row->items().at(1);
    QVERIFY(!item.isNull());
    QCOMPARE(item->type(), MInputMethod::ItemButton);
    QCOMPARE(item->name(), QString("test2"));
    QCOMPARE(item->group(), QString("group2"));
    QCOMPARE(item->priority(), 10);
    QCOMPARE(int(item->showOn()), int(MInputMethod::VisibleWhenSelectingText));
    QCOMPARE(int(item->hideOn()), int(MInputMethod::VisibleUndefined));
    QCOMPARE(item->alignment(), Qt::AlignLeft);
    QCOMPARE(item->text(), QString("text2"));
    QCOMPARE(item->textId(), QString("id2"));
    QCOMPARE(item->isVisible(), true);
    QCOMPARE(item->toggle(), true);
    QCOMPARE(item->pressed(), false);
    QCOMPARE(item->icon(), QString("icon2"));
    QCOMPARE(item->size(), 100);

    QCOMPARE(item->actions().count(), 1);
    QCOMPARE(item->actions().at(0)->type(), MInputMethod::ActionCopy);
    QCOMPARE(item->actions().at(0)->keys(), QString());
    QCOMPARE(item->actions().at(0)->text(), QString());
    QCOMPARE(item->actions().at(0)->command(), QString());
    QCOMPARE(item->actions().at(0)->group(), QString());

    item = row->items().at(2);
    QVERIFY(!item.isNull());
    QCOMPARE(item->type(), MInputMethod::ItemButton);
    QCOMPARE(item->name(), QString("test3"));
    QCOMPARE(item->group(), QString("group4"));
    QCOMPARE(item->priority(), 2);
    QCOMPARE(int(item->showOn()), int(MInputMethod::VisibleAlways));
    QCOMPARE(int(item->hideOn()), int(MInputMethod::VisibleWhenSelectingText));
    QCOMPARE(item->alignment(), Qt::AlignLeft);
    QCOMPARE(item->text(), QString("text4"));
    QCOMPARE(item->textId(), QString("id4"));
    QCOMPARE(item->isVisible(), true);
    QCOMPARE(item->toggle(), false);
    QCOMPARE(item->pressed(), false);
    QCOMPARE(item->icon(), QString("icon4"));
    QCOMPARE(item->size(), 100);

    QCOMPARE(item->actions().count(), 1);
    QCOMPARE(int(item->actions().at(0)->type()), int(MInputMethod::ActionSendCommand));
    QCOMPARE(item->actions().at(0)->keys(), QString());
    QCOMPARE(item->actions().at(0)->text(), QString());
    QCOMPARE(item->actions().at(0)->command(), QString("command4"));
    QCOMPARE(item->actions().at(0)->group(), QString());

    // test portrait part
    QSharedPointer<const MToolbarLayout> portrait = subject->layout(M::Portrait);
    QVERIFY(!portrait.isNull());
    QCOMPARE(portrait->rows().count(), 2);
    row = portrait->rows().at(0);
    QCOMPARE(row->items().count(), 1);

    //itema are shared between layouts
    QVERIFY(landscape->rows().at(0)->items().at(0) == row->items().at(0));

    row = portrait->rows().at(1);
    QCOMPARE(row->items().count(), 2);

    QVERIFY(landscape->rows().at(0)->items().at(1) == row->items().at(0));
    QVERIFY(landscape->rows().at(0)->items().at(2) == row->items().at(1));
}

void Ut_MToolbarData::testLandspaceOnly()
{
    qDebug() << "Try to load file" << Toolbar2;
    QVERIFY2(QFile(Toolbar2).exists(), "toolbar2.xml does not exist");
    bool ok;
    QSharedPointer<const MToolbarRow> row;

    ok = subject->loadNokiaToolbarXml(Toolbar2);
    QVERIFY2(ok, "toolbar2.xml was not loaded correctly");

    QVERIFY(subject->locked() == true);

    //test lanscape part
    QSharedPointer<const MToolbarLayout> landscape = subject->layout(M::Landscape);
    QVERIFY(!landscape.isNull());
    QCOMPARE(landscape->rows().count(), 1);
    row = landscape->rows().at(0);
    QCOMPARE(row->items().count(), 5);

    //verify at least some item's attributes
    QCOMPARE(row->items().at(0)->type(), MInputMethod::ItemButton);
    QCOMPARE(row->items().at(1)->type(), MInputMethod::ItemButton);
    QCOMPARE(row->items().at(2)->type(), MInputMethod::ItemButton);
    QCOMPARE(row->items().at(3)->type(), MInputMethod::ItemLabel);
    QCOMPARE(row->items().at(4)->type(), MInputMethod::ItemButton);

    QVERIFY(subject->layout(M::Landscape) == subject->layout(M::Portrait));
}

void Ut_MToolbarData::testLoadOldXML()
{
    QVERIFY2(QFile(Toolbar3).exists(), "toolbar3.xml does not exist");
    bool ok;

    ok = subject->loadNokiaToolbarXml(Toolbar3);
    QVERIFY2(ok, "toolbar3.xml was not loaded correctly");

    //test lanscape part
    QSharedPointer<const MToolbarLayout> landscape = subject->layout(M::Landscape);
    QVERIFY(!landscape.isNull());
    QCOMPARE(landscape->rows().count(), 1);
    QSharedPointer<const MToolbarRow> row = landscape->rows().at(0);
    QCOMPARE(row->items().count(), 5);

    QVERIFY(subject->layout(M::Landscape) == subject->layout(M::Portrait));
}

QTEST_APPLESS_MAIN(Ut_MToolbarData)

