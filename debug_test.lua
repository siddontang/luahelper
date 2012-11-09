function fun1() 
    local a = 10
    local b = 11
    local c = a + b
    function inner()
        local d = a + b
        db()
    end
    inner()
    return c
end

function fun2() 
    fun1()
end

function fun3()
    fun2()
end

g = {}
g[10] = 10
g["10"] = 11
g.a = 12

print(g["10"])

db()
fun3()

