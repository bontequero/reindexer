#pragma once

#include <deque>
#include <vector>
#include "core/cjson/tagsmatcher.h"
#include "core/keyvalue/keyvalue.h"
#include "core/payload/payloadiface.h"
#include "gason/gason.h"
#include "tools/serializer.h"

using std::vector;

namespace reindexer {

class ItemImpl {
public:
	// Construct empty item
	ItemImpl(PayloadType type, const TagsMatcher &tagsMatcher, const FieldsSet &pkFields = {})
		: payloadType_(type), payloadValue_(type.TotalSize(), 0, type.TotalSize() + 0x100), tagsMatcher_(tagsMatcher), pkFields_(pkFields) {
		tagsMatcher_.clearUpdated();
	}

	ItemImpl(PayloadType type, PayloadValue v, const TagsMatcher &tagsMatcher)
		: payloadType_(type), payloadValue_(v), tagsMatcher_(tagsMatcher) {
		tagsMatcher_.clearUpdated();
	}

	ItemImpl(const ItemImpl &) = delete;
	ItemImpl(ItemImpl &&o) = default;
	ItemImpl &operator=(const ItemImpl &) = delete;
	ItemImpl &operator=(ItemImpl &&) noexcept;

	void SetField(int field, const KeyRefs &krs);
	KeyRef GetField(int field);

	KeyRefs GetValueByJSONPath(const string &jsonPath);

	string_view GetJSON();
	Error FromJSON(const string_view &slice, char **endp = nullptr, bool pkOnly = false);
	Error FromCJSON(ItemImpl *other);

	string_view GetCJSON();
	Error FromCJSON(const string_view &slice, bool pkOnly = false);

	PayloadType Type() { return payloadType_; }
	PayloadValue &Value() { return payloadValue_; }
	Payload GetPayload() { return Payload(payloadType_, payloadValue_); }

	TagsMatcher &tagsMatcher() { return tagsMatcher_; }

	void SetPrecepts(const vector<string> &precepts) { precepts_ = precepts; }
	const vector<string> &GetPrecepts() { return precepts_; }
	void Unsafe(bool enable) { unsafe_ = enable; }

protected:
	// Index fields payload data
	PayloadType payloadType_;
	PayloadValue payloadValue_;
	TagsMatcher tagsMatcher_;
	FieldsSet pkFields_;

	JsonAllocator jsonAllocator_;
	WrSerializer ser_;
	key_string tupleData_;

	vector<string> precepts_;
	bool unsafe_ = false;
	std::deque<std::string> holder_;
};

}  // namespace reindexer
