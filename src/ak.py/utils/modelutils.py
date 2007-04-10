import gtk,gtk.gdk,pygtk,gobject

def match_func(model, iter, data):
   column, key = data # data is a tuple containing column number, key
   value = model.get_value(iter, column)
   return value == key
def search(model, iter, func, data):
   while iter:
       if func(model, iter, data):
           return iter
       result = search(model, model.iter_children(iter), func, data)
       if result: return result
       iter = model.iter_next(iter)
   return None
def get_model_from_list (items, add_blank_entry = False):
    model = gtk.ListStore(str, str)

    if add_blank_entry:
        model.append([0,''])

    if type(items) is tuple:
        items = list(items)

    if type(items) is dict:
        for i in items.keys():
            s = items[i]
            model.append([i,s])
    else:
        if type(items) is str:
            items = [items,] 
        for s in items:
            model.append([s, s])

    return model

def set_model (cb, model, index=0):
    cb.set_model(model)

    if type(cb) == gtk.ComboBoxEntry:
        cb.set_text_column(0)
    elif type(cb) == gtk.ComboBox:
        cell = gtk.CellRendererText()
        cb.pack_start(cell, True)
        cb.add_attribute(cell, 'text', 0)

def magicCombo(model,select,callback,wrap_width=0):
    cb = gtk.ComboBox()
    cb.set_wrap_width(wrap_width)
    set_model(cb, model, select)
    cb.set_active(select)
    cb.connect('changed', callback)
    return cb
