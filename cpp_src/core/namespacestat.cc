
#include "namespacestat.h"
#include "core/cjson/jsonbuilder.h"
#include "gason/gason.h"
#include "tools/jsontools.h"

namespace reindexer {

void NamespaceMemStat::GetJSON(WrSerializer &ser) {
	JsonBuilder builder(ser);

	builder.Put("name", name);
	builder.Put("items_count", itemsCount);

	if (emptyItemsCount) builder.Put("empty_items_count", emptyItemsCount);

	builder.Put("data_size", dataSize);
	builder.Put("updated_unix_nano", int64_t(updatedUnixNano));
	builder.Put("storage_ok", storageOK);
	builder.Put("storage_path", storagePath);

	builder.Put("storage_loaded", storageLoaded);

	builder.Object("total").Put("data_size", Total.dataSize).Put("indexes_size", Total.indexesSize).Put("cache_size", Total.cacheSize);

	{
		auto obj = builder.Object("replication");
		replication.GetJSON(obj);
	}

	{
		auto obj = builder.Object("join_cache");
		joinCache.GetJSON(obj);
	}
	{
		auto obj = builder.Object("query_cache");
		queryCache.GetJSON(obj);
	}

	auto arr = builder.Array("indexes");
	for (auto &index : indexes) {
		auto obj = arr.Object();
		index.GetJSON(obj);
	}
};

void LRUCacheMemStat::GetJSON(JsonBuilder &builder) {
	builder.Put("total_size", totalSize);
	builder.Put("items_count", itemsCount);
	builder.Put("empty_count", emptyCount);
	builder.Put("hit_count_limit", hitCountLimit);
}

void IndexMemStat::GetJSON(JsonBuilder &builder) {
	if (uniqKeysCount) builder.Put("uniq_keys_count", uniqKeysCount);
	if (dataSize) builder.Put("data_size", dataSize);
	if (idsetBTreeSize) builder.Put("idset_btree_size", idsetBTreeSize);
	if (idsetPlainSize) builder.Put("idset_plain_size", idsetPlainSize);
	if (sortOrdersSize) builder.Put("sort_orders_size", sortOrdersSize);
	if (fulltextSize) builder.Put("fulltext_size", fulltextSize);
	if (columnSize) builder.Put("column_size", columnSize);

	if (idsetCache.totalSize || idsetCache.itemsCount || idsetCache.emptyCount || idsetCache.hitCountLimit) {
		auto obj = builder.Object("idset_cache");
		idsetCache.GetJSON(obj);
	}

	builder.Put("name", name);
}

void PerfStat::GetJSON(JsonBuilder &builder) {
	builder.Put("total_queries_count", totalHitCount);
	builder.Put("total_avg_latency_us", totalTimeUs);
	builder.Put("total_avg_lock_time_us", totalLockTimeUs);
	builder.Put("last_sec_qps", avgHitCount);
	builder.Put("last_sec_avg_lock_time_us", avgLockTimeUs);
	builder.Put("last_sec_avg_latency_us", avgTimeUs);
	builder.Put("latency_stddev", stddev);
	builder.Put("min_latency_us", minTimeUs);
	builder.Put("max_latency_us", maxTimeUs);
}

void NamespacePerfStat::GetJSON(WrSerializer &ser) {
	JsonBuilder builder(ser);

	builder.Put("name", name);
	{
		auto obj = builder.Object("updates");
		updates.GetJSON(obj);
	}
	{
		auto obj = builder.Object("selects");
		selects.GetJSON(obj);
	}

	auto arr = builder.Array("indexes");

	for (unsigned i = 0; i < indexes.size(); i++) {
		auto obj = arr.Object();
		indexes[i].GetJSON(obj);
	}
}

void IndexPerfStat::GetJSON(JsonBuilder &builder) {
	builder.Put("name", name);
	{
		auto obj = builder.Object("selects");
		selects.GetJSON(obj);
	}
	{
		auto obj = builder.Object("commits");
		commits.GetJSON(obj);
	}
}

void ReplicationState::GetJSON(JsonBuilder &builder) {
	builder.Put("last_lsn", lastLsn);
	builder.Put("cluster_id", clusterID);
	builder.Put("slave_mode", slaveMode);
	builder.Put("incarnation_counter", incarnationCounter);
	builder.Put("data_hash", dataHash);
}

void ReplicationState::FromJSON(char *json) {
	JsonAllocator jalloc;
	JsonValue jvalue;
	char *endp;

	int status = jsonParse(json, &endp, &jvalue, jalloc);
	if (status != JSON_OK) {
		throw Error(errParseJson, "Malformed JSON with replication state");
	}

	if (jvalue.getTag() != JSON_OBJECT) {
		throw Error(errParseJson, "Expected json object in replication state");
	}

	for (auto elem : jvalue) {
		parseJsonField("last_lsn", lastLsn, elem);
		parseJsonField("cluster_id", clusterID, elem);
		parseJsonField("slave_mode", slaveMode, elem);
		parseJsonField("incarnation_counter", incarnationCounter, elem);
		parseJsonField("data_hash", dataHash, elem);
	}
}

void ReplicationStat::GetJSON(JsonBuilder &builder) {
	ReplicationState::GetJSON(builder);
	if (!slaveMode) {
		builder.Put("wal_count", walCount);
		builder.Put("wal_size", walSize);
	}
}

}  // namespace reindexer
