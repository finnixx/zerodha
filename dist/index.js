"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.TICKER = exports.app = void 0;
const express_1 = __importDefault(require("express"));
const body_parser_1 = __importDefault(require("body-parser"));
exports.app = (0, express_1.default)();
exports.app.use(body_parser_1.default.json());
exports.TICKER = "GOOGLE";
const users = [
    {
        id: "1",
        balances: {
            USD: 10,
            GOOGLE: 100,
        },
    },
    {
        id: "2",
        balances: {
            USD: 30,
            GOOGLE: 120,
        },
    },
];
const bids = [];
const asks = [];
function flipBalances(userId1, userId2, quantity, price) {
    let user1 = users.find((x) => x.id === userId1);
    let user2 = users.find((x) => x.id === userId2);
    if (!user1 || !user2) {
        return;
    }
    user1.balances[exports.TICKER] -= quantity;
    user2.balances[exports.TICKER] += quantity;
    user1.balances["USD"] += quantity * price;
    user2.balances["USD"] -= quantity * price;
}
function fillOrders(side, price, quantity, userId) {
    let remainingQuantity = quantity;
    if (side === "bid") {
        for (let i = asks.length - 1; i >= 0; i--) {
            if (asks[i].price > price)
                continue;
            if (asks[i].quantity > remainingQuantity) {
                asks[i].quantity -= remainingQuantity;
                flipBalances(asks[i].userId, userId, remainingQuantity, price);
                return 0;
            }
            else {
                remainingQuantity -= asks[i].quantity;
                flipBalances(asks[i].userId, userId, asks[i].quantity, price);
                asks.pop();
            }
        }
    }
    else {
        for (let i = bids.length - 1; i >= 0; i--) {
            if (bids[i].price < price)
                continue;
            if (bids[i].quantity > remainingQuantity) {
                bids[i].quantity -= remainingQuantity;
                flipBalances(bids[i].userId, userId, remainingQuantity, price);
                return 0;
            }
            else {
                remainingQuantity -= bids[i].quantity;
                flipBalances(bids[i].userId, userId, bids[i].quantity, price);
                bids.pop();
            }
        }
    }
    return remainingQuantity;
}
exports.app.post("/order", (req, res) => {
    const side = req.body.side;
    const price = req.body.price;
    const quantity = req.body.quantity;
    const userId = req.body.userId;
    const remainingQty = fillOrders(side, price, quantity, userId);
    if (remainingQty === 0) {
        res.json({ filledQuantity: quantity });
        return;
    }
    if (side === "bid") {
        bids.push({
            userId: userId,
            price: price,
            quantity: remainingQty,
        });
        bids.sort((a, b) => (a.price < b.price ? 1 : -1));
    }
    else {
        asks.push({
            userId,
            price,
            quantity: remainingQty,
        });
        asks.sort((a, b) => (a.price < b.price ? 1 : -1));
    }
    res.json({
        filledQuantity: quantity - remainingQty,
    });
});
exports.app.get("/depth", (req, res) => {
    const depth = {};
    for (let i = 0; i < bids.length; i++) {
        if (!depth[bids[i].price]) {
            depth[bids[i].price] = {
                quantity: bids[i].quantity,
                type: "bid",
            };
        }
        else {
            depth[bids[i].price].quantity += bids[i].quantity;
        }
    }
    for (let i = 0; i < asks.length; i++) {
        if (!depth[asks[i].price]) {
            depth[asks[i].price] = {
                quantity: asks[i].quantity,
                type: "ask",
            };
        }
        else {
            depth[asks[i].price].quantity += asks[i].quantity;
        }
    }
    res.json({ depth });
});
exports.app.get("/balance/:userId", (req, res) => {
    const userId = req.params.userId;
    const user = users.find((x) => x.id == userId);
    if (!user) {
        return res.json({
            USD: 0,
            [exports.TICKER]: 0,
        });
    }
    res.json({ balances: user.balances });
});
exports.app.listen(3000);
