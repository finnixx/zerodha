import express from "express";
import bodyParser from "body-parser";

export const app = express();
app.use(bodyParser.json());

interface Balances {
  [key: string]: number;
}

interface User {
  id: string;
  balances: Balances;
}

interface Order {
  userId: string;
  price: number;
  quantity: number;
}

export const TICKER = "GOOGLE";

const users: User[] = [
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

const bids: Order[] = [];
const asks: Order[] = [];

// Dummy fillOrders function to avoid runtime error (you should implement your logic)
function fillOrders(
  side: string,
  price: number,
  quantity: number,
  userId: string
): number {
  return quantity; // for now, assume nothing filled
}

app.post("/order", (req: any, res: any) => {
  const side: string = req.body.side;
  const price: number = req.body.price;
  const quantity: number = req.body.quantity;
  const userId: string = req.body.userId;

  const remainingQty = fillOrders(side, price, quantity, userId);

  if (remainingQty == 0) {
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
  } else {
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

app.get("/depth", (req: any, res: any) => {
  const depth: {
    [price: string]: {
      type: "bid" | "ask";
      quantity: number;
    };
  } = {};

  for (let i = 0; i < bids.length; i++) {
    if (!depth[bids[i].price]) {
      depth[bids[i].price] = {
        quantity: bids[i].quantity,
        type: "bid",
      };
    } else {
      depth[bids[i].price].quantity += bids[i].quantity;
    }
  }

  for (let i = 0; i < asks.length; i++) {
    if (!depth[asks[i].price]) {
      depth[asks[i].price] = {
        quantity: asks[i].quantity,
        type: "ask",
      };
    } else {
      depth[asks[i].price].quantity += asks[i].quantity;
    }
  }

  res.json({ depth });
});

app.get("/balance/:userId", (req: any, res: any) => {
  const userId = req.params.userId;
  const user = users.find((x) => x.id == userId);
  if (!user) {
    return res.json({
      USD: 0,
      [TICKER]: 0,
    });
  }
  res.json({ balances: user.balances });
});

app.listen(3000);

