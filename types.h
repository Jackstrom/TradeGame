#ifndef TYPES_H
#define TYPES_H

namespace TradeGame
{

enum AssetType
{
    NONE, SILVER, GOLD, PLATINUM
};

typedef struct
{
    int silver;
    int gold;
    int platinum;
} Assets;

typedef struct
{
    AssetType sellingType;
    unsigned int sellingVolume;

    AssetType buyingType;
    unsigned int buyingVolume;

} Bid;

typedef struct
{
    Bid trade;
    unsigned int seller;
    unsigned int buyer;
} Trade;

}

#endif // TYPES_H
