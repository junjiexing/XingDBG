//
// Created by xing on 2022-08-06.
//

#include "MemoryView.h"
#include "App.h"
#include "LLDBCore.h"


MemoryView::MemoryView()
	: HexView(nullptr)
{
	setFrameStyle(QFrame::NoFrame);
	//TODO: refresh on debug event

	connect(app(), &App::gotoMemory, this, [this](uint64_t address)
	{
		auto process = core()->getProcess();
		lldb::SBMemoryRegionInfo info;
		auto err = process.GetMemoryRegionInfo(address, info);
		auto addressByteSize = int(process.GetAddressByteSize());
		if (err.Fail())
		{
			app()->w(QString("Get memory region info for address 0x%1 failed: %2")
									.arg(address, addressByteSize * 2, 16, QChar('0'))
									.arg(err.GetCString()));
			return;
		}

		setAddressByteSize(addressByteSize);
		setReadCallBack([](uint64_t base, uint64_t size)
		{
			auto process = core()->getProcess();
			QByteArray ba{qsizetype(size), Qt::Uninitialized};
			lldb::SBError err;
			auto readSize = process.ReadMemory(base, ba.data(), size, err);
			ba.resize(qsizetype(readSize));

			return ba;
		});
		auto base = info.GetRegionBase();
		auto size = info.GetRegionEnd() - base;
		setRange(base, size);
		setCurrentAddress(address);
	});
}
