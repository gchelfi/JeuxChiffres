import Control.Monad (liftM)

data Comb = Base Int | Comb (Op, Comb, Comb) deriving Eq
data Op = Plus | Minus | Mult deriving Eq

instance Show Op where
  show Plus = "+"
  show Minus = "-"
  show Mult = "*"

instance Show Comb where
  show (Base n) = show n
  show (Comb (op,a,b)) = show op ++ "(" ++ show a ++ "," ++ show b ++ ")"

interpreter :: Comb -> Int
interpreter (Base n) = n
interpreter (Comb(op,n1,n2)) = calc op i1 i2
  where calc Plus = (+)
        calc Minus = (-)
        calc Mult = (*)
        i1 = interpreter n1
        i2 = interpreter n2

toComb n = Base n
toCombs = map toComb

toExp op n1 = Comb (op,n1,Base 0)

reserve = toCombs [1..100]



