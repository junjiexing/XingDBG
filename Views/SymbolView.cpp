//
// Created by xing on 2022/8/22.
//

#include "SymbolView.h"
#include <QtWidgets>
#include "App.h"
#include "LLDBCore.h"
#include "Utils.h"

static inline QTableWidgetItem *newItem(const QString &txt)
{
	auto item = new QTableWidgetItem(txt);
	item->setFlags(item->flags() ^ Qt::ItemIsEditable);
	return item;
}

SymbolView::SymbolView()
	: QSplitter(Qt::Horizontal)
{
	auto modulesTable = new QTableWidget(0, 2);
	modulesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	modulesTable->setHorizontalHeaderLabels(QStringList() << tr("Base") << tr("Name"));
	auto searchModuleLab = new QLabel(tr("Search"));    // TODO:
	auto searchModuleEdt = new QLineEdit;
	auto searchModuleIsRegexChk = new QCheckBox(tr("Regex"));

	auto symbolsTable = new QTableWidget(0, 5);
	symbolsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	symbolsTable->setHorizontalHeaderLabels(
		QStringList() << tr("Start address") << tr("End address")
					  << tr("Display name") << tr("Mangled name") << tr("type"));
	auto searchSymbolLab = new QLabel(tr("Search"));    // TODO:
	auto searchSymbolEdt = new QLineEdit;
	auto searchSymbolIsRegexChk = new QCheckBox(tr("Regex"));

	connect(app(), &App::onStopState, this, [=]
	{
		auto target = core()->getTarget();
		auto moduleNum = int(target.GetNumModules());
		modulesTable->setRowCount(moduleNum);
		for (int i = 0; i < moduleNum; ++i) {
			auto m = target.GetModuleAtIndex(i);
			auto base = m.GetObjectFileHeaderAddress().GetLoadAddress(target);
			modulesTable->setItem(i, 0, newItem(Utils::hex(base)));
			auto f = m.GetFileSpec();
			modulesTable->setItem(i, 1, newItem(f.GetFilename()));
		}
	});

	connect(modulesTable->selectionModel(),
			&QItemSelectionModel::currentRowChanged, this,
			[=](const QModelIndex &current, const QModelIndex &)
			{
				auto row = current.row();
				auto target = core()->getTarget();
				if (row >= target.GetNumModules()) {
					app()->w(tr("Invalid module index"));
					return;
				}
				// TODO: 缺少导入导出表
				auto m = target.GetModuleAtIndex(row);
				auto num = int(m.GetNumSymbols());
				symbolsTable->setRowCount(num);
				for (int i = 0; i < num; ++i) {
					auto s = m.GetSymbolAtIndex(i);
					auto startAddr = s.GetStartAddress().GetLoadAddress(target);
					symbolsTable->setItem(i, 0, newItem(Utils::hex(startAddr)));
					auto endAddr = s.GetEndAddress().GetLoadAddress(target);
					symbolsTable->setItem(i, 1, newItem(Utils::hex(endAddr)));
					symbolsTable->setItem(i, 2, newItem(s.GetMangledName()));
					symbolsTable->setItem(i, 3, newItem(s.GetDisplayName()));

					QString typeStr;
					switch (s.GetType()) {
					case lldb::eSymbolTypeAbsolute:
						typeStr = "Absolute";
						break;
					case lldb::eSymbolTypeCode:
						typeStr = "Code";
						break;
					case lldb::eSymbolTypeResolver:
						typeStr = "Resolver";
						break;
					case lldb::eSymbolTypeData:
						typeStr = "Data";
						break;
					case lldb::eSymbolTypeTrampoline:
						typeStr = "Trampoline";
						break;
					case lldb::eSymbolTypeRuntime:
						typeStr = "Runtime";
						break;
					case lldb::eSymbolTypeException:
						typeStr = "Exception";
						break;
					case lldb::eSymbolTypeSourceFile:
						typeStr = "SourceFile";
						break;
					case lldb::eSymbolTypeHeaderFile:
						typeStr = "HeaderFile";
						break;
					case lldb::eSymbolTypeObjectFile:
						typeStr = "ObjectFile";
						break;
					case lldb::eSymbolTypeCommonBlock:
						typeStr = "CommonBlock";
						break;
					case lldb::eSymbolTypeBlock:
						typeStr = "Block";
						break;
					case lldb::eSymbolTypeLocal:
						typeStr = "Local";
						break;
					case lldb::eSymbolTypeParam:
						typeStr = "Param";
						break;
					case lldb::eSymbolTypeVariable:
						typeStr = "Variable";
						break;
					case lldb::eSymbolTypeVariableType:
						typeStr = "VariableType";
						break;
					case lldb::eSymbolTypeLineEntry:
						typeStr = "LineEntry";
						break;
					case lldb::eSymbolTypeLineHeader:
						typeStr = "LineHeader";
						break;
					case lldb::eSymbolTypeScopeBegin:
						typeStr = "ScopeBegin";
						break;
					case lldb::eSymbolTypeScopeEnd:
						typeStr = "ScopeEnd";
						break;
					case lldb::eSymbolTypeAdditional:
						typeStr = "Additional";
						break;
					case lldb::eSymbolTypeCompiler:
						typeStr = "Compiler";
						break;
					case lldb::eSymbolTypeInstrumentation:
						typeStr = "Instrumentation";
						break;
					case lldb::eSymbolTypeUndefined:
						typeStr = "Undefined";
						break;
					case lldb::eSymbolTypeObjCClass:
						typeStr = "ObjCClass";
						break;
					case lldb::eSymbolTypeObjCMetaClass:
						typeStr = "ObjCMetaClass";
						break;
					case lldb::eSymbolTypeObjCIVar:
						typeStr = "ObjCIVar";
						break;
					case lldb::eSymbolTypeReExported:
						typeStr = "ReExported";
						break;
					default:
						typeStr = "Unknown";
					}

					symbolsTable->setItem(i, 4, newItem(typeStr));
				}
			});

	auto leftPane = new QWidget;
	auto vlay = new QVBoxLayout(leftPane);
	vlay->addWidget(modulesTable, 1);
	auto hlay = new QHBoxLayout;
	vlay->addLayout(hlay);
	hlay->addWidget(searchModuleLab);
	hlay->addWidget(searchModuleEdt, 1);
	hlay->addWidget(searchModuleIsRegexChk);

	auto rightPane = new QWidget;
	vlay = new QVBoxLayout(rightPane);
	vlay->addWidget(symbolsTable, 1);
	hlay = new QHBoxLayout;
	vlay->addLayout(hlay);
	hlay->addWidget(searchSymbolLab);
	hlay->addWidget(searchSymbolEdt, 1);
	hlay->addWidget(searchSymbolIsRegexChk);

	addWidget(leftPane);
	addWidget(rightPane);

}
